#ifndef PROTOCOL_VERSION_H
#define PROTOCOL_VERSION_H

#include "dtls.h"

class protocol_version
{
public:
    protocol_version();
    protocol_version(uint8_t major, uint8_t minor);

private:
    endian::big_uint8_t major_;
    endian::big_uint8_t minor_;
};

#endif // PROTOCOL_VERSION_H
