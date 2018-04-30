#include "message.h"

namespace sml
{

message::message(msg_type type)
    : type_(type)
{}

peer_control::peer_control(msg_type type, address addr)
    : message(type), addr_(addr)
{}

add_peer::add_peer(address addr)
    : peer_control(msg_type::add_peer, addr)
{}

del_peer::del_peer(address addr)
    : peer_control(msg_type::del_peer, addr)
{}

new_peer::new_peer(address addr)
    : peer_control(msg_type::new_peer, addr)
{}

stream_control::stream_control(msg_type type, address addr, datagram::id_type id)
    : message(type), addr_(addr), id_(id)
{}

add_stream::add_stream(address addr, datagram::id_type id)
    : stream_control(msg_type::add_stream, addr, id)
{}

del_stream::del_stream(address addr, datagram::id_type id)
    : stream_control(msg_type::del_stream, addr, id)
{}

new_stream::new_stream(address addr, datagram::id_type id)
    : stream_control(msg_type::new_stream, addr, id)
{}

data_message::data_message(msg_type type, address addr, datagram::id_type id, const std::string &data)
    : message(type), addr_(addr), id_(id), data_(data)
{}

recv_data::recv_data(address addr, datagram::id_type id, const std::string &data)
    : data_message(msg_type::recv_data, addr, id, data)
{}

send_data::send_data(address addr, datagram::id_type id, const std::string &data)
    : data_message(msg_type::send_data, addr, id, data)
{}

}
