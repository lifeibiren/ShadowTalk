#ifndef UDP_LAYER_H
#define UDP_LAYER_H

#include "address.h"
#include "config.h"
#include "peer.h"
#include "configuration.h"

namespace sml
{
class udp_layer
{
public:
    typedef function<void(shared_ptr<std::string> msg, shared_ptr<address> addr)> handler_type;
    struct peer_hash_func : std::unary_function<address, std::size_t>
    {
        size_t operator()(const address& addr) const { return addr.hash(); }
    };
    typedef unordered_map<address, shared_ptr<peer>, peer_hash_func> peer_map_type;

    udp_layer(service& a_service);
    void send_to(shared_ptr<std::string> msg, shared_ptr<address> addr, handler_type handler);
    void register_handler(handler_type handler, shared_ptr<address> addr_hits);
    void register_handler(handler_type handler);

    void add_peer(const address& addr);
    void del_peer(const address& addr);
    shared_ptr<peer> get_peer(const address& addr);

    configuration& conf() const;
private:
    void start_receive();
    void handle_receive(const boost::system::error_code& error_message, std::size_t bytes_transferred);
    void handle_send(const boost::system::error_code& error_message, shared_ptr<std::string> message,
        shared_ptr<address> addr, handler_type handler);

    service& service_;
    struct hash_func : std::unary_function<shared_ptr<address>, std::size_t>
    {
        size_t operator()(const shared_ptr<address>& x) const { return x->hash(); }
    };

    boost::unordered_map<shared_ptr<address>, handler_type, hash_func> handler_map_;

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
