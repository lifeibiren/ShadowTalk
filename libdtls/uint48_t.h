#ifndef UINT48_T_H
#define UINT48_T_H

#include <stdint.h>

class uint48_t
{
public:
    uint48_t();
    uint48_t(uint8_t *bytes);
    uint48_t(uint64_t val);
    uint48_t(const uint48_t &val);
    uint48_t &operator=(const uint48_t &val);
    uint48_t &operator=(const uint64_t val);
    uint64_t value() const;
private:
    uint16_t bytes_[3];
};

#endif // UINT48_T_H
