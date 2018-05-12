#include "udp_layer.h"
#include "sml.h"

namespace sml
{
udp_layer::udp_layer(service& a_service)
    : service_(a_service)
    , socket_(service_.io_context(),
              asio::ip::udp::endpoint(asio::ip::udp::v4(), service_.conf().port()))
{
    start_receive();
}

void udp_layer::start_receive()
{
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_layer::handle_receive, this, boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}

void udp_layer::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error || error == boost::asio::error::message_size)
    {
        shared_ptr<std::string> received_message(new std::string(recv_buffer_.c_array(), bytes_transferred));
        /*
         * distribute udp datagram to handlers
         */
        shared_ptr<address> addr = make_shared<address>(remote_endpoint_);
        auto it = handler_map_.find(addr);
        if (it != handler_map_.end())
        {
            (it->second)(received_message, addr);
        }
        for (auto it = default_handler_list_.begin(); it != default_handler_list_.end(); it++)
        {
            (*it)(received_message, addr);
        }

        /*
         * feed peers
         */
        peer_map_type::iterator peer_it = peer_map_.find(*addr);
        if (peer_it == peer_map_.end())
        {
            peer_map_.insert(peer_map_type::value_type(*addr, make_shared<peer>(service_.io_context(), *this, *addr)));
            peer_it = peer_map_.find(*addr);
            // send new peer message
            output_ring.put(make_shared<new_peer>(*addr));
        }
        peer_it->second->feed(received_message);

        start_receive();
    }
}

void udp_layer::send_to(boost::shared_ptr<std::string> msg, boost::shared_ptr<address> addr, handler_type handler)
{
    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(addr->ip()), addr->port());
    socket_.async_send_to(boost::asio::buffer(*msg), endpoint,
        boost::bind(&udp_layer::handle_send, this, boost::asio::placeholders::error, msg, addr, handler));
}

void udp_layer::register_handler(handler_type handler, shared_ptr<address> addr_hits)
{
    handler_map_[addr_hits] = handler;
}

void udp_layer::register_handler(handler_type handler)
{
    default_handler_list_.push_back(handler);
}

void udp_layer::add_peer(const address& addr)
{
    peer_map_.insert(peer_map_type::value_type(addr, make_shared<peer>(service_.io_context(), *this, addr)));
}

void udp_layer::del_peer(const address& addr)
{
    size_t ret = peer_map_.erase(addr);
    if (ret < 1)
    {
        output_ring.put(make_shared<error_message>("no such peer\n"));
    }
    else if (ret > 0)
    {
        throw unknown_error();
    }
}

shared_ptr<peer> udp_layer::get_peer(const address& addr)
{
    peer_map_type::iterator it = peer_map_.find(addr);
    if (it == peer_map_.end())
    {
        return nullptr;
    }
    return it->second;
}

configuration& udp_layer::conf() const
{
    return service_.conf();
}

void udp_layer::handle_send(const boost::system::error_code& error, shared_ptr<std::string> message,
    shared_ptr<address> addr, handler_type handler)
{
    if (!error)
    {
        handler(message, addr);
    }
}
}
