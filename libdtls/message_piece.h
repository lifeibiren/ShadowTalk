#ifndef MESSAGE_PIECE_H
#define MESSAGE_PIECE_H

#include "hash.h"
#include <string>

namespace whisper
{
class message_piece
{
public:
    message_piece();

private:
    hash hash_;
    std::string bytes_;
};
} // namespace shadowtalk

#endif // MESSAGE_PIECE_H
