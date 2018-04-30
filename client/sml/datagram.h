#ifndef DATAGRAM_H_
#define DATAGRAM_H_

#include "config.h"

namespace sml
{
class message;
class datagram
{
public:
    datagram();
    datagram(shared_ptr<std::string> bytes);
    operator shared_ptr<std::string>() const;
    operator std::string() const;

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
    typedef uint32_t offset_type;
    typedef uint32_t length_type;

    id_type id_;
    msg_type type_;
    length_type offset_;
    offset_type length_;
    std::string data_;
};
} // namespace shadowtalk

#endif // MESSAGE_PIECE_H
