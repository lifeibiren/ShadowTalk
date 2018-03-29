#ifndef ADDRESS_H
#define ADDRESS_H

#include "config.h"

namespace sml {
class address
{
public:
    address(const std::string &ip_addr, uint16_t port);
    address(const asio::ip::udp::endpoint &endpoint);
    const std::string &ip() const;
    void set_ip(const std::string &ip);
    uint16_t port() const;
    void set_port(uint16_t port);
    bool operator==(const address &val) const;
private:
    std::string ip_addr_;
    uint16_t port_;
};
}

#endif // ADDRESS_H
