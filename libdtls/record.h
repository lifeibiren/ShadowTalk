#ifndef RECORD_LAYER_H
#define RECORD_LAYER_H


#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/static_assert.hpp>
#include <cstdint>
#include "uint48_t.h"
#include "byte_string.h"

class record
{
public:
    enum class content_type_type : std::uint8_t {
        change_cipher_spec = 20,
        alert = 21,
        handshake = 22,
        application_data = 23
    };

    record() : version_{254, 255} /* DTLS v1.2 */, epoch_(0),
        sequence_number_((std::uint64_t)0), length_(0) { }
    virtual ~record() { }
    virtual void from_bytes(boost::shared_ptr<byte_string> bytes); //big endian
    virtual boost::shared_ptr<byte_string> to_bytes() const; //big endian

    content_type_type type() const;
    void set_type(content_type_type new_type);
    std::uint16_t epoch() const;
    void set_epoch(std::uint16_t new_epoch);
    uint48_t sequence() const;
    void set_sequence(uint48_t new_sequence);
    std::uint16_t length() const;
private:
    typedef struct protocol_version{
        std::uint8_t major;
        std::uint8_t minor;
    } protocol_version_type;

    typedef struct header{
        content_type_type type;
        protocol_version_type version;
        std::uint16_t epoch;
        uint48_t sequence_number;
        std::uint16_t length;
    } __attribute__((packed)) header_type;

    content_type_type type_;
    protocol_version_type version_;
    std::uint16_t epoch_;
    uint48_t sequence_number_;
    std::uint16_t length_;

    /* ensure header_type is packed correctly */
    BOOST_STATIC_ASSERT(
        sizeof(header_type) ==
        sizeof(content_type_type) +
        sizeof(protocol_version_type) +
        sizeof(std::uint16_t) +
        sizeof(uint48_t) +
        sizeof(std::uint16_t)
    );
};

#endif // RECORD_LAYER_H
