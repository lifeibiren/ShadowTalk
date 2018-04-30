#include "datagram.h"
#include "byte_string.h"
namespace sml
{
datagram::datagram() {}
datagram::datagram(shared_ptr<std::string> bytes)
{
    byte_string b_str(*bytes);
    b_str >> id_;
    b_str >> type_;
    b_str >> offset_;
    b_str >> length_;
    b_str >> data_;
}
datagram::operator shared_ptr<std::string>() const
{
    byte_string b_str;
    b_str << id_;
    b_str << type_;
    b_str << offset_;
    b_str << length_;
    b_str << data_;
    return shared_ptr<std::string>(new std::string(b_str.to_std_string()));
}
datagram::operator std::string() const
{
    return string_format("id: %d\ntype: %d\noffset: %d\nlength %d\ndata: %s\n",
                         id_, type_, offset_, length_, data_.c_str());
}
} // namespace shadowtalk
