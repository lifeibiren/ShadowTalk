#include "udp_layer.h"
#include "service.h"

namespace sml
{
udp_layer::udp_layer(service &a_service)
    : service_(a_service)
    , socket_(service_.io_context(), asio::ip::udp::endpoint(asio::ip::udp::v4(), service_.conf().port()))
    , default_filter_([](shared_ptr<std::string>, const address &){return true;})
{
    start_receive();
}

void udp_layer::start_receive()
{
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_layer::handle_receive, this, boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void udp_layer::handle_receive(const boost::system::error_code &error, std::size_t bytes_transferred)
{
    if (!error || error == boost::asio::error::message_size)
    {
        shared_ptr<std::string> received_message(new std::string(recv_buffer_.c_array(), bytes_transferred));
        /*
         * distribute udp datagram to handlers
         */
        address addr(remote_endpoint_);
        auto it = filter_map_.find(addr);
        bool feed_peer = true;
        if (it != filter_map_.end())
        {
            feed_peer = (it->second)(received_message, addr);
        }
        else
        {
            feed_peer = default_filter_(received_message, addr);
        }

        if (feed_peer)
        {
            /*
             * feed peers
             */
            peer_map_type::iterator peer_it = peer_map_.find(addr);
            if (peer_it == peer_map_.end())
            {
                peer_map_.insert(peer_map_type::value_type(addr, make_shared<peer>(service_.io_context(), *this, addr)));
                peer_it = peer_map_.find(addr);
            }
            peer_it->second->feed(received_message);
        }

        start_receive();
    }
}

void udp_layer::send_to(boost::shared_ptr<std::string> msg, const address &addr, handler_type handler)
{
    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(addr.ip()), addr.port());
    socket_.async_send_to(boost::asio::buffer(*msg), endpoint,
        boost::bind(&udp_layer::handle_send, this, boost::asio::placeholders::error, msg, addr, handler));
}

void udp_layer::register_filter(filter_type filter, const address &addr_hits)
{
    filter_map_[addr_hits] = filter;
}

void udp_layer::register_default_filter(filter_type filter)
{
    default_filter_ = filter;
}

void udp_layer::add_peer(const address &addr)
{
    peer_map_.insert(peer_map_type::value_type(addr, make_shared<peer>(service_.io_context(), *this, addr)));
}

void udp_layer::del_peer(const address &addr)
{
    size_t ret = peer_map_.erase(addr);
    if (ret < 1)
    {
        output_ring.put(make_shared<error_message>("no such peer\n"));
    }
    else if (ret > 1)
    {
        throw unknown_error();
    }
}

shared_ptr<peer> udp_layer::get_peer(const address &addr)
{
    peer_map_type::iterator it = peer_map_.find(addr);
    if (it == peer_map_.end())
    {
        return nullptr;
    }
    return it->second;
}

configuration &udp_layer::conf() const
{
    return service_.conf();
}

void udp_layer::handle_send(const boost::system::error_code &error, shared_ptr<std::string> message,
    const address &addr, handler_type handler)
{
    if (!error)
    {
        handler(message, addr);
    }
}
}
