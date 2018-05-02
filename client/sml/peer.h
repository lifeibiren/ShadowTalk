#ifndef PEER_H
#define PEER_H

#include "config.h"
#include "encrypt_layer.h"
#include "message.h"
#include "peer.h"
#include "stream.h"
#include "udp_layer.h"

namespace sml
{
class peer
{
public:
    typedef boost::function<void(shared_ptr<message>)> handler_type;
    typedef function<void(shared_ptr<stream>)> stream_handler_type;
    typedef datagram::id_type id_type;

    peer(asio::io_context& io_context, udp_layer& udp_layer, const address& addr);
    shared_ptr<stream> add_stream(id_type id);
    void del_stream(id_type id);
    shared_ptr<stream> get_stream(id_type id);
    void async_accept_new_stream(stream_handler_type handler);
    std::vector<id_type> new_stream_id_vec() const;

    void feed(shared_ptr<std::string> encrypted_msg);

    void send_datagram(shared_ptr<datagram> data);
    const address& addr() const;

private:
    void packet_handler(shared_ptr<std::string> packet, shared_ptr<address> addr);
    void handshake(shared_ptr<std::string> msg, shared_ptr<address> addr);

    void send_public_params();
    void handle_public_params(shared_ptr<std::string> msg, shared_ptr<address> addr);
    void send_fake_shared_key();
    void handle_fake_shared_key(shared_ptr<std::string> msg, shared_ptr<address> addr);
    void generate_key();

    //#define initial 0x0
    //#define sent_public_params 0x1
    //#define received_public_params 0x2
    //#define public_params_exchanged (sent_public_params | received_public_params)
    //#define sent_fake_shared_key 0x4
    //#define received_fake_shared_key 0x8
    //#define fake_shared_key_exchanged (sent_fake_shared_key | received_fake_shared_key)
    //#define established 0xf
    uint16_t current_state_;

    asio::io_context& io_context_;
    udp_layer& udp_layer_;
    const address addr_;
    shared_ptr<std::string> key_;
    shared_ptr<encrypt_layer> encrypt_layer_;

    typedef std::map<id_type, shared_ptr<stream>> stream_map_type;
    stream_map_type stream_map_;
    std::vector<id_type> new_stream_id_;
    std::vector<stream_handler_type> new_stream_handler_;

    shared_ptr<message> send_msg_, recv_msg_;
};
} // namespace sml

#endif // TRANSPORT_LAYER_H
