#ifndef ADDRESS_H
#define ADDRESS_H

#include "config.h"

namespace sml {
class address
{
public:
    address(shared_ptr<std::string> ip_addr, uint16_t port);
    shared_ptr<std::string> ip() const;
    uint16_t port() const;
    bool operator==(const address &val) const;
private:
    shared_ptr<std::string> ip_addr_;
    uint16_t port_;
};
}

#endif // ADDRESS_H
