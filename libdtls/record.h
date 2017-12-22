#ifndef RECORD_LAYER_H
#define RECORD_LAYER_H

#include <cstdint>
#include <string>
#include <boost/shared_ptr.hpp>
class record
{
public:
    typedef enum {
        change_cipher_spec = 20,
        alert = 21,
        handshake = 22,
        application_data = 23
    } content_type;

    typedef struct {
        std::uint8_t major;
        std::uint8_t minor;
    } protocol_version_type;

    typedef struct {
        content_type type;
        protocol_version_type version;
        std::uint16_t epoch;
        std::uint16_t sequence_number[3];
        std::uint16_t length;
    } dtls_plaintext_header_type;

    record();
    record(boost::shared_ptr<std::string> bytes); //big endian
    void from_bytes(boost::shared_ptr<std::string> bytes); //big endian
    boost::shared_ptr<std::string> to_bytes() const; //big endian
private:


    const protocol_version_type version_ = {3, 3}; /* TLS v1.2 */
    dtls_plaintext_header_type dtls_plaintext_header_;
    std::string fragment_;
};

#endif // RECORD_LAYER_H
