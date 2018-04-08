#ifndef TRANSPORT_LAYER_H
#define TRANSPORT_LAYER_H

#include "config.h"
#include "message.h"
#include "message_piece.h"
#include "udp_layer.h"
namespace sml {
class transport_layer
{
public:
    typedef boost::function<void (shared_ptr<message>)> handler_type;
    transport_layer(shared_ptr<udp_layer> udp_layer_);
    void send(shared_ptr<message> msg, handler_type handler);
    void recv(shared_ptr<message> msg, handler_type handler);
private:
    shared_ptr<udp_layer> udp_layer_;
};
} // namespace sml

#endif // TRANSPORT_LAYER_H
