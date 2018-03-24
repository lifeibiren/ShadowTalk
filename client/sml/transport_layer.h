#ifndef TRANSPORT_LAYER_H
#define TRANSPORT_LAYER_H

#include "sml.h"
#include "message.h"

namespace sml {
class transport_layer
{
public:
    typedef function<void (shared_ptr<message>)> message_handler_type;
    transport_layer();
    void send(shared_ptr<message> msg);
    void register_new_message_callback(message_handler_type handler);
    void register_receive_complete_callback(message_handler_type handler);
private:
    unordered_map<uint32_t, shared_ptr<message>> message_table_;
};
} // namespace sml

#endif // TRANSPORT_LAYER_H
