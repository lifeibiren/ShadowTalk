#ifndef UDP_LAYER_H
#define UDP_LAYER_H

#include "config.h"
#include "address.h"
namespace sml {
class udp_layer
{
public:
    typedef function<void (shared_ptr<std::string> msg, shared_ptr<std::string> ip_addr, uint16_t port)> receive_handler_type;

    udp_layer(asio::io_service& io_service, uint16_t port);
    void send_to(shared_ptr<std::string> msg, shared_ptr<address> addr);
    void register_handler(receive_handler_type handler, shared_ptr<address> addr_hits);
    void register_handler(receive_handler_type handler);
private:
    void start_receive();
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_send(boost::shared_ptr<std::string> msg, const boost::system::error_code& error);


    struct hash_func : std::unary_function<shared_ptr<address>, std::size_t>
    {
        size_t operator()(const shared_ptr<address> &x) const
        {
            std::size_t seed = 0;
            boost::hash_combine(seed, x->ip());
            boost::hash_combine(seed, x->port());
            return seed;
        }
    };
    unordered_map<shared_ptr<address>, receive_handler_type, hash_func> map_;

    /* data structure used to store incoming packets */
    const static uint32_t buffer_size_ = 1500;
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint remote_endpoint_;
    array<char, buffer_size_> recv_buffer_;
};
}

#endif // UDP_LAYER_H
