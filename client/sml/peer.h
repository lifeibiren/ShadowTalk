#ifndef PEER_H
#define PEER_H

#include "config.h"
#include "dh_key_agreement.h"
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
    //    void async_accept_new_stream(stream_handler_type handler);
    //    std::vector<id_type> new_stream_id_vec() const;
    void feed(shared_ptr<std::string> encrypted_msg);
    void send_datagram(shared_ptr<datagram> data);

    const address& addr() const;

private:
    void send_raw_datagram(shared_ptr<datagram> msg);
    void send_raw_datagram_with_retransmit(shared_ptr<datagram> msg);
    void do_send(shared_ptr<std::string> bytes);
    void retransmit_raw_datagram(shared_ptr<datagram> msg);
    //    void packet_handler(shared_ptr<std::string> packet, shared_ptr<address> addr);
    bool peer_handler(shared_ptr<datagram> new_datagram);
    void send_ack();
    void send_public_key();
    void handle_public_key(shared_ptr<datagram> new_datagram);
    void send_partial_shared_key();
    void handle_partal_shared_key(shared_ptr<std::string> msg);
    void generate_key_final();

    const static uint8_t initial = 0x0;
    const static uint8_t sent_public_key = 0x1;
    const static uint8_t receved_public_key_ack = 0x2;
    const static uint8_t received_public_key = 0x4;
    const static uint8_t public_key_exchanged = sent_public_key | received_public_key | receved_public_key_ack;
    const static uint8_t shared_key_agreement = 0x8;
    const static uint8_t established = public_key_exchanged | shared_key_agreement;

    uint8_t current_state_;

    asio::io_context& io_context_;
    asio::deadline_timer timer_;
    udp_layer& udp_layer_;
    const address addr_;
    dh_key_agreement dh_key_agreement_;
    std::string peer_spub_;
    shared_ptr<datagram> to_send_; // used in key agreement
    shared_ptr<std::string> key_;
    shared_ptr<encrypt_layer> encrypt_layer_;

    typedef std::map<id_type, shared_ptr<stream>> stream_map_type;
    stream_map_type stream_map_;
    //    std::vector<id_type> new_stream_id_;
    //    std::vector<stream_handler_type> new_stream_handler_;

    shared_ptr<message> send_msg_, recv_msg_;
};
} // namespace sml

#endif // TRANSPORT_LAYER_H
