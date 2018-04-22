#ifndef MESSAGE_H
#define MESSAGE_H

#include "config.h"
#include "address.h"
#include <map>

namespace sml {
class datagram;
class message
{
public:
    friend class peer;

    typedef uint32_t id_type;

    id_type id() const;
    void feed(shared_ptr<datagram> piece);
    id_type id_;
    address addr_;
    std::string data_;
};
}

#endif // MESSAGE_H
