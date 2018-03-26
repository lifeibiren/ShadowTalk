#include "address.h"

namespace sml {
address::address(shared_ptr<std::string> ip_addr, uint16_t port) :
    ip_addr_(ip_addr), port_(port)
{

}
shared_ptr<std::string> address::ip() const
{
    return ip_addr_;
}
uint16_t address::port() const
{
    return port_;
}
bool address::operator==(const address &val) const
{
    if (*ip_addr_ == *(val.ip_addr_) && port_ == val.port_) {
        return true;
    } else {
        return false;
    }
}
}
