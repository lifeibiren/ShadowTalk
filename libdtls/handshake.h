#ifndef HANDSHAKE_PROTOCOL_H
#define HANDSHAKE_PROTOCOL_H

#include <boost/endian/endian.hpp>
#include <boost/integer.hpp>

#include "body.h"

using namespace boost;

class handshake
{
public:
    enum class handshake_type : uint8_t
    {
        hello_request = 0,
        client_hello = 1,
        server_hello = 2,
        hello_verify_request = 3,
        certificate = 11,
        server_key_exchange = 12,
        certificate_request = 13,
        server_hello_done = 14,
        certificate_verify = 15,
        client_key_exchange = 16,
        finished = 20,
        invalid = 255
    };

    handshake();

    handshake_type msg_type_;
    endian::big_uint24_t length_;
    endian::big_uint16_t message_seq_;
    endian::big_uint24_t fragment_offset_;
    endian::big_uint24_t fragment_length_;
    body body_;
};

#endif // HANDSHAKE_PROTOCOL_H
