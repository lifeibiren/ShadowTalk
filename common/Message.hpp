#pragma once

#include <cinttypes>
#include <optional>
#include <stdexcept>
#include <vector>

#include <boost/endian/arithmetic.hpp>

#include <msgpack.hpp>

namespace whisper {

enum MessageType : uint8_t { INVALID, NOP, PING, PONG, LIST_PEERS, PEER_LIST, SEND_MSG, PEER_MSG };

struct Message {
    MessageType type;
    std::string dst;
    std::string src;
    std::vector<char> bytes;

    MSGPACK_DEFINE(type, src, dst, bytes);

    Message() noexcept
        : type(INVALID) {}

    static Message CreateMessage(MessageType type, std::vector<char> data = {}) {
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
