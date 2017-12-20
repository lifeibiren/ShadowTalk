#include "peer.h"

namespace shadowtalk {
peer::peer()
{
    last_beat_ = std::time(NULL);
}
peer::peer(const std::string &address,  unsigned int port) :
    peer()
{
    address_ = address;
    port_ = port;
}
const std::string &peer::address() const
{
    return address_;
}
unsigned int peer::port() const
{
    return port_;
}
bool peer::dead() const
{
    return std::time(NULL) - last_beat_ > 30;
}
std::string peer::to_string() const
{
    return address_ + " " + std::to_string(port_) + '\n';
}
//bool peer::operator=(const peer &r_peer)
//{
//    address_ = r_peer.address_;
//    port_ = r_peer.port_;
//    name_ = r_peer.name_;
//    pub_key_ = r_peer.pub_key_;
//    last_beat_ = ;
//}
bool peer::operator==(const peer &r_peer) const
{
    if (address_.compare(r_peer.address_) == 0 &&
            port_ == r_peer.port_) {
        return true;
    } else {
        return false;
    }
}
bool peer::operator!=(const peer &r_peer) const
{
    return !(*this == r_peer);
}
bool peer::operator<(const peer &r_peer) const
{
    if (*this == r_peer) {
        return false;
    }
    int comp_result = address_.compare(r_peer.address_);
    if (comp_result < 0 ||
            (comp_result == 0 && port_ < r_peer.port_)) {
        return true;
    } else {
        return false;
    }
}
bool peer::operator>(const peer &r_peer) const
{
    if (*this == r_peer) {
        return false;
    } else {
        return !(*this < r_peer);
    }
}
} //namespace shadowtalk
