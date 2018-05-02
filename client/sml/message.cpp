#include "message.h"
#include "peer.h"
#include "stream.h"
#include "udp_layer.h"

namespace sml
{

message::message(msg_type type)
    : type_(type)
{}
message::~message()
{}

info_message::info_message(const std::string &info)
    : info_(info)
{}

peer_message::peer_message(msg_type type, address addr)
    : message(type), addr_(addr)
{}

add_peer::add_peer(address addr)
    : peer_message(msg_type::add_peer, addr)
{}

void add_peer::operator()(udp_layer &a_udp_layer)
{
    std::cout<<"add peer"<<std::endl;
    a_udp_layer.add_peer(addr_);
}

del_peer::del_peer(address addr)
    : peer_message(msg_type::del_peer, addr)
{}

void del_peer::operator()(udp_layer &a_udp_layer)
{
    a_udp_layer.del_peer(addr_);
}

new_peer::new_peer(address addr)
    : peer_message(msg_type::new_peer, addr)
{}

stream_message::stream_message(msg_type type, address addr, datagram::id_type id)
    : message(type), addr_(addr), id_(id)
{}

add_stream::add_stream(address addr, datagram::id_type id)
    : stream_message(msg_type::add_stream, addr, id)
{}

void add_stream::operator()(udp_layer &a_udp_layer)
{
    shared_ptr<peer> _peer = a_udp_layer.get_peer(addr_);
    if (_peer)
    {
        _peer->add_stream(id_);
    }
}

del_stream::del_stream(address addr, datagram::id_type id)
    : stream_message(msg_type::del_stream, addr, id)
{}

void del_stream::operator()(udp_layer &a_udp_layer)
{
    shared_ptr<peer> _peer = a_udp_layer.get_peer(addr_);
    if (_peer)
    {
        _peer->del_stream(id_);
    }
}

new_stream::new_stream(address addr, datagram::id_type id)
    : stream_message(msg_type::new_stream, addr, id)
{}

data_message::data_message(msg_type type, address addr, datagram::id_type id, const std::string &data)
    : message(type), addr_(addr), id_(id), data_(data)
{}

recv_data::recv_data(address addr, datagram::id_type id, const std::string &data)
    : data_message(msg_type::recv_data, addr, id, data)
{}

recv_data::operator std::string()
{
    return data_;
}

send_data::send_data(address addr, datagram::id_type id, const std::string &data)
    : data_message(msg_type::send_data, addr, id, data)
{}

void send_data::operator()(udp_layer &a_udp_layer)
{
    shared_ptr<peer> _peer = a_udp_layer.get_peer(addr_);
    if (_peer)
    {
        shared_ptr<stream> _stream = _peer->get_stream(id_);
        if (_stream)
        {
            _stream->send(data_);
        }
    }
}

error_message::error_message(const std::string &info)
    : message(msg_type::error), info_message(info)
{}

}
