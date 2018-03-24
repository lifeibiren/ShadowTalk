#include "udp_server.h"

#include <iostream>

extern boost::signals2::signal<void()> sig;
std::string make_daytime_string()
{
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

udp_server::udp_server(boost::asio::io_service& io_service, unsigned short port)
    : socket_(io_service, udp::endpoint(udp::v4(), port))
{
    start_receive();
}
void udp_server::send_to(boost::shared_ptr<std::string> message, const boost::asio::ip::udp::endpoint& endpoint)
{
    socket_.async_send_to(boost::asio::buffer(*message), endpoint,
        boost::bind(&udp_server::handle_send, this, message, boost::asio::placeholders::error));
}
void udp_server::register_receive_handler(const datagram_signal::slot_type& slot)
{
    receive_signal_.connect(slot);
}

void udp_server::register_send_handler(const datagram_signal::slot_type& slot)
{
    send_signal_.connect(slot);
}

void udp_server::start_receive()
{
    socket_.async_receive_from(boost::asio::buffer(recv_buffer_), remote_endpoint_,
        boost::bind(&udp_server::handle_receive, this, boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
}
void udp_server::handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred)
{
    if (!error || error == boost::asio::error::message_size)
    {
        boost::shared_ptr<std::string> received_message(new std::string(recv_buffer_.c_array(), bytes_transferred));
        receive_signal_(received_message, remote_endpoint_, error);

        start_receive();
    }
}
void udp_server::handle_send(boost::shared_ptr<std::string> message, const boost::system::error_code& error)
{
    send_signal_(message, remote_endpoint_, error);
}
