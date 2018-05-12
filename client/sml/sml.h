#ifndef SML_H
#define SML_H

/*
 * -------------
 * |  stream   |  ---- correspond to a data stream
 * -------------
 * |   peer    |  ---- correspond to a remote peer
 * -------------
 * |  encrypt  |  ---- reusable, pure functional, handle each datagram
 * -------------
 * |    udp    |  ---- correspond to a local port
 * -------------
 * |  service  |  ---- warp around this whole protocol
 * -------------
 *
 * service: providing external interfaces
 *
 * stream: high-level interfaces used to send and receive data with remote peers
 *
 * peer: handle handshake, and distribute datagrams of each stream
 *
 * encrypt: provide interfaces to encrypt end decrypt data.
 *
 * udp_layer: wrap around UDP socket
 *
 * datagram: UDP datagram which is the unit of data transfer
 *
 */

#include "config.h"
#include "message.h"
#include "peer.h"
#include "udp_layer.h"
#include "configuration.h"

namespace sml
{
class service
{
public:
    service(asio::io_context& io_context, const configuration &conf);

    shared_ptr<message> query();
    void post(shared_ptr<message>);

    asio::io_context& io_context();

    configuration& conf();
private:
    void msg_handler();
    configuration conf_;
    asio::io_context& io_context_;
    uint16_t port_;
    udp_layer udp_layer_;
};
} // namespace sml

#endif // SML_H
