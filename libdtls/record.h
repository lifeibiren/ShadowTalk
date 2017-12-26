#ifndef RECORD_LAYER_H
#define RECORD_LAYER_H


#include <string>
#include <boost/shared_ptr.hpp>
#include <stdint.h>
#include <uint48_t.h>
class record
{
public:
    record();
    record(boost::shared_ptr<std::string> bytes); //big endian
    void from_bytes(boost::shared_ptr<std::string> bytes); //big endian
    boost::shared_ptr<std::string> to_bytes() const; //big endian
private:
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
        uint16_t epoch;
        uint48_t sequence_number;
        uint16_t length;
    } __attribute__((packed)) plaintext_header_type;

    typedef struct {
        content_type type;
        protocol_version_type version;
        uint16_t epoch;
        uint48_t sequence_number;
        uint16_t length;
    } compressed_header_type;

    typedef struct {
        content_type type;
        protocol_version_type version;
        uint16_t epoch;
        uint48_t sequence_number;
        uint16_t length;
    } ciphertext_header_type;

    const protocol_version_type version_ = {254, 255}; /* DTLS v1.2 */
};

#endif // RECORD_LAYER_H
