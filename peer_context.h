#ifndef PEER_CONTEXT_H
#define PEER_CONTEXT_H

#include <boost/shared_ptr.hpp>
#include <list>
#include "hash.h"
#include "message.h"
#include "message_piece.h"

namespace whisper {
class peer_context
{
public:
    peer_context();
    void receive_handler(boost::shared_ptr<std::string> bytes);
    bool is_finished() const;
    boost::shared_ptr<message> get_message() const;
private:
    typedef enum peer_state {
        FINISHED,
        PREPARING,
        SENDING,
        WAITING
    } peer_state_type;

    peer_state_type peer_state_;
    hash hash_of_last_sent_;

    std::list<message_piece> send_queue_;
    std::list<message_piece> recv_queue_;
};
} //namespace shadowtalk

#endif // PEER_CONTEXT_H
