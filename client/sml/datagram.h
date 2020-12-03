#ifndef DATAGRAM_H_
#define DATAGRAM_H_

#include "config.h"

namespace sml {
class datagram {
public:
    datagram();
    datagram(const datagram &val);
    datagram(shared_ptr<std::string> bytes);
    datagram &operator=(const datagram &val);
    bool operator==(const datagram &val) const;
    bool operator!=(const datagram &val) const;
    operator shared_ptr<std::string>() const;
    operator std::string() const;
    void encrypt_payload(encrypt_layer &el);
    void decrypt_payload(encrypt_layer &el);

    enum class msg_type : uint8_t {
        keep_alive,
        ack,
        hello,
        public_key,
        partial_shared_key, // temparorily not used
        echo,
        echo_back,
        data,
        data_ack,
        abort
    };
    typedef uint32_t id_type;
    typedef uint32_t offset_type;
    typedef uint32_t length_type;

    msg_type type_;
    id_type id_;
    length_type offset_;
    offset_type length_;
    std::string payload_;

    static shared_ptr<datagram> create_keep_alive();
    static shared_ptr<datagram> create_hello(const std::string &my_id);
    static shared_ptr<datagram> create_public_key(const std::string &pub_key);
    static shared_ptr<datagram> create_ack(id_type id, offset_type offset);
    static shared_ptr<datagram> create_echo(const std::string &content);
    static shared_ptr<datagram> create_echo_back(const datagram &echo_datagram);
    static shared_ptr<datagram> create_abort();
};
} // namespace shadowtalk

#endif // MESSAGE_PIECE_H
