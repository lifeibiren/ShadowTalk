#ifndef SML_H
#define SML_H

/*
 * -------------
 * |    sml    |  ---- correspond to a local port
 * -------------
 * |  stream   |  ---- correspond to a data stream
 * -------------
 * | transport |  ---- correspond to a remote peer
 * -------------
 * |  encrypt  |  ---- reusable, pure functional, handle each datagram
 * -------------
 * |    udp    |  ---- associate with a sml instance
 * -------------
 *
 *
 * sml : initialize udp_layer, handle handshake, create peers
 *
 * peer : high-level interfaces used to send and receive data with remote peers
 *
 * transport : handle a single message associated with a specific peer
 *
 * encrypt : provide interfaces to encrypt end decrypt data.
 *
 * udp_layer : provide low-level interfaces to send and receive data.
 *
 * message : the minimal complete data unit
 *
 * message_piece : UDP datagram
 *
 */

#include "config.h"
#include "encrypt_layer.h"
#include "transport_layer.h"

namespace sml
{
class sml
{
public:
    sml();
};
} // namespace sml

#endif // SML_H
