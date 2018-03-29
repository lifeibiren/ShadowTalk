#include "udp_layer.h"

namespace sml {
udp_layer::udp_layer(asio::io_service& io_service, uint16_t port)
    : socket_(io_service, asio::ip::udp::endpoint(asio::ip::udp::v4(), port))
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
        /**
         * distribute udp datagram here
         */
        shared_ptr<address> addr = make_shared<address>(remote_endpoint_);
        auto it = handler_map_.find(addr);
        if (it != handler_map_.end()) {
            (it->second)(received_message, addr);
        }
        for (auto it = default_handler_list_.begin(); it != default_handler_list_.end(); it ++) {
            (*it)(received_message, addr);
        }
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
void udp_layer::handle_send(const boost::system::error_code& error, shared_ptr<std::string> message, shared_ptr<address> addr, handler_type handler)
{
    if (!error)
    {
        handler(message, addr);
    }
}
}
