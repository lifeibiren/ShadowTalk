#include "peer.h"
#include "message_piece.h"

namespace sml {
peer::peer(std::string addr, uint8_t port) :
    addr_(addr), port_(port)
{
}

void peer::send(shared_ptr<message> msg, handler_type handler)
{
    auto it = make_shared<transport_layer>(udp_layer_);
    send_list_.push_back(it);
    it->send(msg, handler);
}

void peer::recv(shared_ptr<message> msg, handler_type handler)
{
    auto it = make_shared<transport_layer>(udp_layer_);
    recv_list_.push_back(it);
    it->recv(msg, handler);
}
}
