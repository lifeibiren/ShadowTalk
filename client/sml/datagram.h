#ifndef DATAGRAM_H_
#define DATAGRAM_H_

#include "config.h"

namespace sml
{
class message;
class datagram
{
public:
    datagram();
    datagram(const datagram& val);
    datagram(shared_ptr<std::string> bytes);
    datagram& operator=(const datagram& val);
    operator shared_ptr<std::string>() const;
    operator std::string() const;
    void encrypt_payload(encrypt_layer& el);
    void decrypt_payload(encrypt_layer& el);

    typedef uint32_t id_type;
    enum class msg_type : uint8_t
    {
        keep_alive,
        ack,
        public_key,
        partial_shared_key, // temparorily not used
        echo,
        data,
        data_ack,
        abort
    };
    typedef uint32_t offset_type;
    typedef uint32_t length_type;

    msg_type type_;
    id_type id_;
    length_type offset_;
    offset_type length_;
    std::string payload_;

    static shared_ptr<datagram> create_keep_alive();
    static shared_ptr<datagram> create_public_key(const std::string& pub_key);
    static shared_ptr<datagram> create_ack(id_type id, offset_type offset);
};
} // namespace shadowtalk

#endif // MESSAGE_PIECE_H
