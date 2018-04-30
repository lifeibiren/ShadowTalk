#ifndef SML_H
#define SML_H

/*
 * -------------
 * |    sml    |  ---- correspond to a local port
 * -------------
 * |  stream   |  ---- correspond to a data stream
 * -------------
 * |   peer    |  ---- correspond to a remote peer
 * -------------
 * |  encrypt  |  ---- reusable, pure functional, handle each datagram
 * -------------
 * |    udp    |  ---- associate with a sml instance
 * -------------
 *
 *
 * sml : initialize udp_layer, handle handshake, create peers
 *
 * stream : high-level interfaces used to send and receive data with remote peers
 *
 * transport : handle a single message associated with a specific peer
 *
 * encrypt : provide interfaces to encrypt end decrypt data.
 *
 * udp_layer : provide low-level interfaces to send and receive data.
 *
 * message : the minimal complete data unit
 *
 * datagram : UDP datagram
 *
 */

#include "config.h"
#include "peer.h"
#include "udp_layer.h"
#include "message.h"

namespace sml
{
class service : public enable_shared_from_this<service>
{
public:
    typedef function<void(shared_ptr<peer>)> handler_type;

    service(uint16_t port);
    void init();
    void start();
    shared_ptr<peer> create_peer(const address& addr);
    void async_accept_peer(handler_type handler);

    std::list<message> query();
    void post(std::list<message>);
private:
    void new_peer_handler(shared_ptr<std::string> msg, shared_ptr<address> addr);
    uint16_t port_;
    udp_layer udp_layer_;
    std::vector<handler_type> handler_list_;
    std::vector<shared_ptr<peer>> new_peer_list_;
};
} // namespace sml

#endif // SML_H
