#pragma once

#include <cinttypes>
#include <optional>
#include <stdexcept>
#include <vector>

#include <boost/endian/arithmetic.hpp>

#include <msgpack.hpp>

namespace whisper {

enum MessageType : uint8_t {
    INVALID,
    NOP,

    // Messages used to keep alive
    PING, // S -> C
    PONG, // C -> S

    // Messages used to query online peers
    LIST_PEERS, // C -> S
    PEER_LIST, // S -> C

    // Messages used to send message to other peers
    SEND_MSG, // C -> S
    PEER_MSG, // S -> C

    // Messages used to initiate point-to-point connections
    //    WANT TO CONNECT
    // A -----------------> Server
    //    CONNECT KEY
    // A <----------------> Server
    //    CONNECTED BY
    // B <----------------- Server
    //
    // Start new websocket with KEY
    // A -----------------> Server
    // B -----------------> Server
    WANT_TO_CONNECT, // C -> S
    CONNECT_KEY, // S -> C
    CONNECTED_BY, // S -> C
};

struct PeerAddr {
    std::string ip_;
    std::uint16_t port_;

    PeerAddr() noexcept
        : port_(0) {}
    PeerAddr(std::string ip, std::uint16_t port)
        : ip_(ip)
        , port_(port) {}

    MSGPACK_DEFINE(ip_, port_);
};

using PeerList = std::vector<PeerAddr>;

struct Message {
    MessageType type;
    std::string dst;
    std::string src;
    std::vector<char> bytes;

    PeerList peer_list;

    PeerAddr to_connect;

    MSGPACK_DEFINE(type, src, dst, bytes, peer_list, to_connect);

    Message() noexcept
        : type(INVALID) {}

    static Message CreateMessage(
        MessageType type, std::vector<char> data = {}) {
        Message r;
        r.type = type;
        r.bytes = data;
        return r;
    }

    static Message CreateMessage(MessageType type, std::string data) {
        Message r;
        r.type = type;
        r.bytes = std::vector<char>(data.data(), data.data() + data.size());
        return r;
    }
};

struct MessageCtx {
    msgpack::unpacker unp;

    std::list<Message> Feed(const std::vector<char> &bytes) {
        std::list<Message> ret;

        unp.reserve_buffer(bytes.size());

        memcpy(unp.buffer(), bytes.data(), bytes.size());

        unp.buffer_consumed(bytes.size());

        msgpack::object_handle result;
        // Message pack data loop
        while (unp.next(result)) {
            ret.emplace_back(result.get().as<Message>());
        }

        return ret;
    }
};

}

MSGPACK_ADD_ENUM(whisper::MessageType);
