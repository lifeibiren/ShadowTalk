#ifndef MESSAGE_H
#define MESSAGE_H

#include "address.h"
#include "byte_string.h"
#include "config.h"
#include "datagram.h"

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
        recv_data,
        error
    };

    message() {}
    message(msg_type type);
    virtual ~message();

protected:
    msg_type type_;
};

class control_message
{
public:
    virtual void operator()(udp_layer& a_udp_layer) = 0;
};

class info_message
{
public:
    info_message(const std::string& info);

private:
    const std::string info_;
};

class peer_message : public message
{
public:
    peer_message(msg_type type, address addr);

protected:
    address addr_;
};

class add_peer : public peer_message, public control_message
{
public:
    add_peer(address addr);
    virtual void operator()(udp_layer& a_udp_layer);
};

class del_peer : public peer_message, public control_message
{
public:
    del_peer(address addr);
    void operator()(udp_layer& a_udp_layer);
};

class new_peer : public peer_message
{
public:
    new_peer(address addr);
    void operator()(udp_layer& a_udp_layer);
};

class stream_message : public message
{
public:
    stream_message(msg_type type, address addr, datagram::id_type id);

protected:
    address addr_;
    datagram::id_type id_;
};

class add_stream : public stream_message, public control_message
{
public:
    add_stream(address addr, datagram::id_type id);
    void operator()(udp_layer& a_udp_layer);
};
class del_stream : public stream_message, public control_message
{
public:
    del_stream(address addr, datagram::id_type id);
    void operator()(udp_layer& a_udp_layer);
};

class new_stream : public stream_message
{
public:
    new_stream(address addr, datagram::id_type id);
};

class data_message : public message
{
public:
    data_message(msg_type type, address addr, datagram::id_type id, const std::string& data);

protected:
    address addr_;
    datagram::id_type id_;
    std::string data_;
};

class recv_data : public data_message
{
public:
    recv_data(address addr, datagram::id_type id, const std::string& data);
    operator std::string();
};

class send_data : public data_message, public control_message
{
public:
    send_data(address addr, datagram::id_type id, const std::string& data);
    void operator()(udp_layer& a_udp_layer);
};

class error_message : public message, public info_message
{
public:
    error_message(const std::string& info);

protected:
    std::string info_;
};
}
#endif // MESSAGE_H
