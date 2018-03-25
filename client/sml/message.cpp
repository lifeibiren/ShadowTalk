#include "message.h"

namespace sml {
message::message()
{

}
message::id_type message::id() const
{
    return id_;
}
void message::feed(shared_ptr<message_piece> piece)
{

}
}
