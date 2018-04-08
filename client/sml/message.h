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
    enum class msg_type : uint8_t
    {
        key_exchange,
        request,
        confirm,
        data,
        acknowledge,
        abort
    };

    id_type id() const;
    void feed(shared_ptr<datagram> piece);
private:
    id_type id_;
    address addr_;
    std::map<uint32_t, datagram> pieces;
};
}

#endif // MESSAGE_H
