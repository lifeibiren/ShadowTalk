#ifndef DTLS_COMPRESSED_H
#define DTLS_COMPRESSED_H

#include "ciphertext.h"
#include "plaintext.h"
#include "record.h"

class compressed : public record
{
public:
    compressed()
        : fragment(new byte_string)
    {}
    compressed(const plaintext& val);
    compressed(const ciphertext& val);

private:
    boost::shared_ptr<byte_string> compression(boost::shared_ptr<byte_string> bytes);
    boost::shared_ptr<byte_string> decompression(boost::shared_ptr<byte_string> bytes);

    boost::shared_ptr<byte_string> fragment;
};

#endif // DTLS_COMPRESSED_H
