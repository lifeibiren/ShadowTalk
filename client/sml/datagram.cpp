#include "datagram.h"
#include "byte_string.h"
#include "encrypt_layer.h"
namespace sml
{
datagram::datagram()
    : id_(0)
    , type_(msg_type::keep_alive)
    , offset_(0)
    , length_(0)
{}

datagram::datagram(const datagram& val)
{
    *this = val;
}

datagram::datagram(shared_ptr<std::string> bytes)
{
    byte_string b_str(*bytes);
    b_str >> type_;
    b_str >> id_;
    b_str >> offset_;
    b_str >> length_;
    b_str >> payload_;
}

datagram& datagram::operator=(const datagram& val)
{
    id_ = val.id_;
    type_ = val.type_;
    offset_ = val.offset_;
    length_ = val.length_;
    payload_ = val.payload_;
    return *this;
}

bool datagram::operator==(const datagram& val) const
{
    return (id_ == val.id_) && (type_ == val.type_) && (offset_ == val.offset_) && (length_ == val.length_) &&
            (payload_ == val.payload_);
}

bool datagram::operator!=(const datagram& val) const
{
   return !(*this == val);
}

datagram::operator shared_ptr<std::string>() const
{
    byte_string b_str;
    b_str << type_;
    b_str << id_;
    b_str << offset_;
    b_str << length_;
    b_str << payload_;
    return shared_ptr<std::string>(new std::string(b_str.to_std_string()));
}
datagram::operator std::string() const
{
    return string_format(
        "id: %d\ntype: %d\noffset: %d\nlength %d\npayload: %s\n", id_, type_, offset_, length_, payload_.c_str());
}

void datagram::encrypt_payload(encrypt_layer& el)
{
    if (payload_.size() != 0)
    {
        payload_ = el.encrypt(payload_);
    }
}

void datagram::decrypt_payload(encrypt_layer& el)
{
    if (payload_.size() != 0)
    {
        payload_ = el.decrypt(payload_);
    }
}

shared_ptr<datagram> datagram::create_keep_alive()
{
    shared_ptr<datagram> p = boost::make_shared<datagram>();
    p->type_ = msg_type::keep_alive;
    return p;
}

shared_ptr<datagram> datagram::create_hello(const std::string &my_id)
{
    shared_ptr<datagram> p = boost::make_shared<datagram>();
    p->type_ = msg_type::hello;
    p->payload_ = my_id;
    p->length_ = p->payload_.size();
    return p;
}

shared_ptr<datagram> datagram::create_public_key(const std::string& pub_key)
{
    shared_ptr<datagram> p = boost::make_shared<datagram>();
    p->type_ = msg_type::public_key;
    p->offset_ = 0;
    p->length_ = pub_key.size();
    p->payload_ = pub_key;
    return p;
}
shared_ptr<datagram> datagram::create_ack(id_type id, offset_type offset)
{
    shared_ptr<datagram> p = boost::make_shared<datagram>();
    p->type_ = msg_type::ack;
    p->offset_ = offset;
    return p;
}

shared_ptr<datagram> datagram::create_echo(const std::string &content)
{
    shared_ptr<datagram> p = boost::make_shared<datagram>();
    p->type_ = msg_type::echo;
    p->length_ = content.size();
    p->payload_ = content;
    return p;
}

shared_ptr<datagram> datagram::create_echo_back(const datagram &echo_datagram)
{
    shared_ptr<datagram> p = boost::make_shared<datagram>(echo_datagram);
    p->type_ = msg_type::echo_back;
    return p;
}
} // namespace shadowtalk
