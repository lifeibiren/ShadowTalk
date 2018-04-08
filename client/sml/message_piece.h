#ifndef MESSAGE_PIECE_H
#define MESSAGE_PIECE_H

#include "config.h"
#include "message.h"

namespace sml
{
class message;
class datagram
{
public:
    datagram();
    datagram(shared_ptr<std::string> bytes);
    operator shared_ptr<std::string> () const;

    message::id_type id_;
    message::msg_type type_;
    uint32_t length_;
    uint32_t offset_;
    std::string data_;
};
} // namespace shadowtalk

#endif // MESSAGE_PIECE_H
