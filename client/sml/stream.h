#ifndef STREAM_H
#define STREAM_H

#include "config.h"
#include "message_piece.h"

namespace sml
{
class stream
{
public:
    typedef function<void(shared_ptr<datagram> data)> send_callback_type;
    typedef datagram::id_type id_type;

    stream(id_type id, send_callback_type callback);
    void feed(shared_ptr<datagram> packet);

    stream &operator>>(shared_ptr<std::string> &data);
    stream &operator<<(shared_ptr<std::string> &data);
private:
    void send_one_datagram();
    void retransmit();
    void send_ack();

    enum class stream_state_type : uint8_t
    {
        initial = 0,
        sent_hello,
        recv_hello,
        established,
        closed
    } state_;

    asio::deadline_timer timer;

    id_type id_;
    static const size_t segment_max_ = 1000;
    send_callback_type send_callback_;

    datagram::offset_type send_offset_, recv_offset_;
    datagram::length_type send_length_, recv_length_;
    shared_ptr<std::string> send_data_, recv_data_;

    typedef std::map<datagram::offset_type, shared_ptr<datagram>> datagram_map_type;
    datagram_map_type send_datagram_queue_, recv_datagram_queue_;
};
}

#endif // STREAM_H
