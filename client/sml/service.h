#ifndef SERVICE_H_
#define SERVICE_H_

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
#include "configuration.h"
#include "message.h"
#include "peer.h"
#include "udp_layer.h"

namespace sml {
class service {
public:
    service(asio::io_context &io_context, const configuration &conf);

    shared_ptr<message> query();
    void post(shared_ptr<message>);

    asio::io_context &io_context();

    configuration &conf();

    shared_ptr<std::vector<address>> peer_list() const;

private:
    void msg_handler();
    void query_server(const boost::system::error_code &ec);
    bool server_datagram_filter(
        shared_ptr<std::string> msg, const address &addr);

    configuration conf_;
    asio::io_context &io_context_;
    udp_layer udp_layer_;
    asio::deadline_timer timer_;
    shared_ptr<std::vector<address>> peer_list_;
    shared_ptr<std::vector<address>> new_peer_list_;
    mutable mutex mutex_;
};
} // namespace sml

#endif // SML_H
