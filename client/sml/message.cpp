#include "message.h"

namespace sml {
message::id_type message::id() const
{
    return id_;
}
void message::feed(shared_ptr<datagram> piece)
{

}
}
