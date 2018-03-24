#ifndef RANDOM_H
#define RANDOM_H

#include "dtls.h"

namespace dtls
{
class random
{
public:
    random();

private:
    endian::big_uint32_t gmt_unix_time_;
    array<uint8_t, 28> random_bytes;
};
} // namespace dtls

#endif // RANDOM_H
