#ifndef UDP_LAYER_H
#define UDP_LAYER_H

#include "address.h"
#include "config.h"
#include "peer.h"

namespace sml
{
class service;
class udp_layer
{
public:
    typedef function<void(shared_ptr<std::string> msg, shared_ptr<address> addr)> handler_type;
    struct peer_hash_func: std::unary_function<address, std::size_t>
    {
        size_t operator()(const address &addr) const
        {
            return addr.hash();
        }
    };
    typedef unordered_map<address, shared_ptr<peer>, peer_hash_func> peer_map_type;

    udp_layer(uint16_t port);
    void send_to(shared_ptr<std::string> msg, shared_ptr<address> addr, handler_type handler);
    void register_handler(handler_type handler, shared_ptr<address> addr_hits);
    void register_handler(handler_type handler);

private:
    void start_receive();
    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred);
    void handle_send(const boost::system::error_code& error, shared_ptr<std::string> message, shared_ptr<address> addr,
        handler_type handler);

    struct hash_func: std::unary_function<shared_ptr<address>, std::size_t>
    {
        size_t operator()(const shared_ptr<address>& x) const
        {
            return x->hash();
        }
    };

    unordered_map<shared_ptr<address>, handler_type, hash_func> handler_map_;

    peer_map_type peer_map_;


    std::list<handler_type> default_handler_list_;

    /* data structure used to store incoming packets */
    const static uint32_t buffer_size_ = 1500;
    asio::ip::udp::socket socket_;
    asio::ip::udp::endpoint remote_endpoint_;
    array<char, buffer_size_> recv_buffer_;
};
}

#endif // UDP_LAYER_H
