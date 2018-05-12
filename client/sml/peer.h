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
    void feed(shared_ptr<std::string> data);
    void send_datagram(shared_ptr<datagram> data);
    const address& addr() const;
private:
    void send_raw_datagram(shared_ptr<datagram> msg);
    void send_raw_datagram_with_retransmit(shared_ptr<datagram> msg);
    void retransmit_raw_datagram(shared_ptr<datagram> msg, const system::error_code& ec);
    void do_send(shared_ptr<std::string> bytes);

    void send_ack();

    void hello_handler(shared_ptr<datagram> new_datagram);
    void send_hello();
    void received_hello_handler(shared_ptr<datagram> new_datagram);
    void received_hello_ack_handler(shared_ptr<datagram> new_datagram);
    void hello_complete_handler();

    void public_key_exchange_handler(shared_ptr<datagram> new_datagram);
    void send_public_key();
    void received_public_key_handler(shared_ptr<datagram> new_datagram);
    void received_public_key_ack_handler(shared_ptr<datagram> new_datagram);
    void public_key_exchange_complete_handler();
    void generate_shared_key();

    void send_echo();
    void received_echo_handler(shared_ptr<datagram> new_datagram);
    void received_echo_back_handler(shared_ptr<datagram> new_datagram);
    void echo_handler(shared_ptr<datagram> new_datagram);

    void established_handler(shared_ptr<datagram> new_datagram);

    // TODO: use shared key to agree a new key
    void send_partial_shared_key();
    void handle_partal_shared_key(shared_ptr<std::string> msg);

    const static uint32_t initial = 0;
    const static uint32_t sent_hello = 1U << 0;
    const static uint32_t received_hello_ack = 1U << 1;
    const static uint32_t received_hello = 1U << 2;
    const static uint32_t hello_complete = sent_hello | received_hello_ack | received_hello;
    const static uint32_t sent_public_key = 1U << 3;
    const static uint32_t received_public_key_ack = 1U << 4;
    const static uint32_t received_public_key = 1U << 5;
    const static uint32_t public_key_exchanged = sent_public_key | received_public_key | received_public_key_ack;
    const static uint32_t shared_key_agreement = 1U << 6;
    const static uint32_t established = hello_complete | public_key_exchanged | shared_key_agreement;
    const static uint32_t wait_for_echo = 1U << 7;

    uint32_t current_state_;
    typedef boost::function<void (shared_ptr<datagram>)> datagram_handler_type;
    datagram_handler_type datagram_handler_;
    asio::io_context& io_context_;
    asio::deadline_timer timer_;
    udp_layer& udp_layer_;
    const address addr_;
    dh_key_agreement dh_key_agreement_;
    std::string peer_spub_;
    shared_ptr<datagram> sent_echo_datagram_;
    shared_ptr<datagram> to_send_; // used in key agreement
    shared_ptr<std::string> key_;
    shared_ptr<encrypt_layer> encrypt_layer_;

    typedef std::map<id_type, shared_ptr<stream>> stream_map_type;
    stream_map_type stream_map_;
    shared_ptr<message> send_msg_, recv_msg_;
};
} // namespace sml

#endif // TRANSPORT_LAYER_H
