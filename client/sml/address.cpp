#include "address.h"

namespace sml
{
address::address(const std::string& ip_addr, uint16_t port)
    : ip_addr_(ip_addr)
    , port_(port)
{}
address::address(const asio::ip::udp::endpoint& endpoint)
{
    ip_addr_ = endpoint.address().to_string();
    port_ = endpoint.port();
}
const std::string& address::ip() const
{
    return ip_addr_;
}
void address::set_ip(const std::string& ip)
{
    ip_addr_ = ip;
}
uint16_t address::port() const
{
    return port_;
}
void address::set_port(uint16_t port)
{
    port_ = port;
}
bool address::operator==(const address& val) const
{
    if (ip_addr_ == val.ip_addr_ && port_ == val.port_)
    {
        return true;
    }
    else
    {
        return false;
    }
}
size_t address::hash() const
{
    std::size_t seed = 0;
    boost::hash_combine(seed, ip_addr_);
    boost::hash_combine(seed, port_);
    return seed;
}
}
