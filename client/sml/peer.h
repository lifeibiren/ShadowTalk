#ifndef PEER_H
#define PEER_H

#include "config.h"
#include "message.h"
#include "transport_layer.h"
#include "udp_layer.h"
namespace sml {
class peer
{
public:
    typedef boost::function<void (shared_ptr<message>)> handler_type;
    peer(std::string addr, uint8_t port);
    void send(shared_ptr<message> msg, handler_type handler);
    void recv(shared_ptr<message> msg, handler_type handler);
private:
    shared_ptr<udp_layer> udp_layer_;
    std::list<shared_ptr<transport_layer>> recv_list_;
    std::list<shared_ptr<transport_layer>> send_list_;
    std::string addr_;
    uint8_t port_;
};
}

#endif // PEER_H
