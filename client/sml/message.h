#ifndef MESSAGE_H
#define MESSAGE_H

#include "address.h"
#include "config.h"
#include "datagram.h"
#include "byte_string.h"

namespace sml
{
class message
{
public:
    enum class msg_type : uint8_t
    {
        add_peer,
        del_peer,
        new_peer,
        add_stream,
        del_stream,
        new_stream,
        send_data,
        recv_data
    };

    message(msg_type type);
    virtual void operator()() = 0;
    virtual ~message();
protected:
    msg_type type_;
};

class peer_control : public message
{
public:
    peer_control(msg_type type, address addr);
protected:
    address addr_;
};

class add_peer : public peer_control
{
public:
    add_peer(address addr);
};

class del_peer: public peer_control
{
public:
    del_peer(address addr);
};

class new_peer: public peer_control
{
public:
    new_peer(address addr);
};

class stream_control: public message
{
public:
    stream_control(msg_type type, address addr, datagram::id_type id);
protected:
    address addr_;
    datagram::id_type id_;
};

class add_stream: public stream_control
{
public:
    add_stream(address addr, datagram::id_type id);
};
class del_stream: public stream_control
{
public:
    del_stream(address addr, datagram::id_type id);
};

class new_stream: public stream_control
{
public:
    new_stream(address addr, datagram::id_type id);
};

class data_message: public message
{
public:
    data_message(msg_type type, address addr, datagram::id_type id, const std::string &data);
protected:
    address addr_;
    datagram::id_type id_;
    std::string data_;
};

class recv_data: public data_message
{
public:
    recv_data(address addr, datagram::id_type id, const std::string &data);
};

class send_data: public data_message
{
public:
    send_data(address addr, datagram::id_type id, const std::string &data);
};
}
#endif // MESSAGE_H
