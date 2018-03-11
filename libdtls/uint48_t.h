#ifndef UINT48_T_H
#define UINT48_T_H

#include <cinttypes>

class __attribute__((packed)) uint48_t
{
public:
    uint48_t();
    uint48_t(std::uint8_t *bytes);
    uint48_t(std::uint64_t val);
    uint48_t(const uint48_t &val);
    uint48_t &operator=(const uint48_t &val);
    uint48_t &operator=(std::uint64_t val);
    operator uint64_t();
    std::uint64_t value() const;

    uint48_t operator +(uint64_t val);
    uint48_t operator -(uint64_t val);
private:
    std::uint16_t bytes_[3];
};

#endif // UINT48_T_H
