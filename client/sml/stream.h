#ifndef STREAM_H
#define STREAM_H

#include "config.h"
#include "datagram.h"

namespace sml
{
class stream
{
public:
    typedef function<void(shared_ptr<datagram> data)> send_callback_type;
    typedef datagram::id_type id_type;

    stream(asio::io_context &io_context, peer &a_peer, id_type id);
    void feed(shared_ptr<datagram> packet);
    void send(const std::string &data);
    void send_one_piece();
private:
    void send_one_datagram();
    void retransmit();
    void send_ack();

//    enum class stream_state_type : uint8_t
//    {
//        initial = 0,
//        sent_hello,
//        recv_hello,
//        established,
//        closed
//    } state_;

    asio::io_context &io_context_;
    peer &peer_;
    asio::deadline_timer timer;

    id_type id_;
    static const size_t segment_max_ = 1000;

    datagram::offset_type send_offset_, recv_offset_;
    datagram::length_type send_length_, recv_length_;

    std::list<std::string> send_data_list_;

    typedef std::map<datagram::offset_type, shared_ptr<datagram>> datagram_map_type;
    datagram_map_type send_datagram_queue_, recv_datagram_queue_;
};
}

#endif // STREAM_H
