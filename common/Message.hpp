#pragma once

#include <cinttypes>
#include <optional>
#include <stdexcept>
#include <vector>

#include <boost/endian/arithmetic.hpp>

namespace whisper {
struct Message {
    enum MessageType : uint8_t { NOP, PING, PONG, LIST_PEERS, PEER_LIST, SEND_MSG };

    std::uint32_t length;
    MessageType type;
    std::vector<char> bytes;

    static Message CreateMessage(MessageType type, std::vector<char> data = {}) {
        Message r;
        r.length = data.size() + sizeof(length) + sizeof(type);
        r.type = type;
        r.bytes = data;
        return r;
    }

    static Message FromBytes(const char *data, std::size_t len) {
        const char *end = data + len;
        if (len < sizeof(length) + sizeof(type)) {
            throw std::runtime_error("Message: data too short");
        }
        boost::endian::big_uint32_buf_t en_buf;
        memcpy(en_buf.data(), data, 4);

        Message r;
        r.length = en_buf.value();
        data += 4;
        r.type = static_cast<MessageType>(*data);
        data++;

        if (r.length != len) {
            throw std::runtime_error("Message: data corrupted");
        }
        r.bytes = std::vector<char>(data, end);
        return r;
    }
    static std::vector<char> ToBytes(const Message &m) {
        std::vector<char> r;
        boost::endian::big_uint32_buf_t buf(m.length);
        r.emplace_back(*(const char *)buf.data());
        r.emplace_back(*(const char *)(buf.data() + 1));
        r.emplace_back(*(const char *)(buf.data() + 2));
        r.emplace_back(*(const char *)(buf.data() + 3));
        r.emplace_back(static_cast<char>(m.type));
        r.insert(r.end(), m.bytes.begin(), m.bytes.end());
        return r;
    }
};

struct MessageCtx {
    std::vector<char> buf_;
    std::optional<std::uint32_t> len_;

    std::optional<Message> Feed(const std::vector<char> &bytes) {
        buf_.insert(buf_.end(), bytes.begin(), bytes.end());
        if (buf_.size() < 4) {
            return {};
        }
        if (!len_) {
            boost::endian::big_uint32_buf_t en_buf;
            memcpy(en_buf.data(), buf_.data(), 4);
            len_ = en_buf.value();
        }
        if (buf_.size() < len_) {
            return {};
        }
        Message m = Message::FromBytes(buf_.data(), len_.value());
        buf_.erase(buf_.begin(), buf_.begin() + len_.value());
        if (buf_.capacity() > 1024 * 1024) {
            buf_.shrink_to_fit();
        }
        len_.reset();
        return m;
    }
};

}