#ifndef MESSAGE_PIECE_H
#define MESSAGE_PIECE_H

#include "config.h"
#include "message.h"

namespace sml
{
class message;
class message_piece
{
public:
    message_piece();
    message::id_type message_id() const;
private:
    message::id_type id_;
    sptr_string bytes_;
};
} // namespace shadowtalk

#endif // MESSAGE_PIECE_H
