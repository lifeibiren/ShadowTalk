#ifndef MESSAGE_PIECE_H
#define MESSAGE_PIECE_H

#include "sml.h"

namespace sml
{
class message_piece
{
public:
    message_piece();
private:
    sptr_string bytes_;
};
} // namespace shadowtalk

#endif // MESSAGE_PIECE_H
