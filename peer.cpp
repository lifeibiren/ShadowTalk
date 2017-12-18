#include "peer.h"

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
std::string peer::address() const
{
    return address_;
}
unsigned int peer::port() const
{
    return port_;
}
bool peer::dead() const
{
    return std::time(NULL) - last_beat_ > 10;
}
std::string peer::to_string() const
{
    return address_ + " " + std::to_string(port_) + '\n';
}
