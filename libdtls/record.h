#ifndef RECORD_LAYER_H
#define RECORD_LAYER_H

#include <dtls.h>

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
        sequence_((std::uint64_t)0), length_(0) { }
    virtual ~record() { }
    virtual void from_bytes(boost::shared_ptr<byte_string> bytes); //big endian
    virtual boost::shared_ptr<byte_string> to_bytes() const; //big endian

    content_type_type type() const;
    void set_type(content_type_type new_type);
    uint16_t epoch() const;
    void set_epoch(std::uint16_t new_epoch);
    uint48_t sequence() const;
    void set_sequence(uint48_t new_sequence);
    uint16_t length() const;
protected:
    content_type_type type_;
    protocol_version version_;
    endian::big_uint16_t epoch_;
    endian::big_uint48_t sequence_;
    endian::big_uint16_t length_;
};

#endif // RECORD_LAYER_H
