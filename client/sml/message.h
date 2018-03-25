#ifndef MESSAGE_H
#define MESSAGE_H

#include "config.h"

namespace sml {
class message_piece;
class message
{
public:
    typedef uint32_t id_type;
    message();
    id_type id() const;
    void feed(shared_ptr<message_piece> piece);
private:
    id_type id_;
};
}

#endif // MESSAGE_H
