#ifndef UDP_SERVER_H
#define UDP_SERVER_H

#include <list>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <boost/signals2.hpp>

using boost::asio::ip::udp;

std::string make_daytime_string();

class udp_server
{
public:
    typedef boost::signals2::signal<void (boost::shared_ptr<std::string> message,
                                          boost::asio::ip::udp::endpoint &remote_endpoint,
                                          const boost::system::error_code& error)> datagram_signal;
    typedef datagram_signal::slot_type message_signal_handler;

    udp_server(boost::asio::io_service& io_service, unsigned short port);
    void send_to(boost::shared_ptr<std::string> message,
                 const boost::asio::ip::udp::endpoint &endpoint);
    void register_receive_handler(const message_signal_handler &slot);
    void register_send_handler(const message_signal_handler &slot);
private:
    void start_receive();

    void handle_receive(const boost::system::error_code& error,
                        std::size_t bytes_transferred);

    void handle_send(boost::shared_ptr<std::string> message,
                     const boost::system::error_code& error);

    const static boost::uint32_t buffer_size_ = 1500;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, buffer_size_> recv_buffer_;

    datagram_signal receive_signal_;
    datagram_signal send_signal_;
};

#endif // UDP_SERVER_H
