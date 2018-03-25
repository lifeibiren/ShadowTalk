#include "message_piece.h"

namespace sml
{
message_piece::message_piece() {}
message::id_type message_piece::message_id() const
{
    return id_;
}
} // namespace shadowtalk
