#include "uint48_t.h"
#include <string.h>
uint48_t::uint48_t() : bytes_{0, 0, 0}
{
}
uint48_t::uint48_t(std::uint64_t val)
{
    memcpy(bytes_, &val, sizeof(bytes_));
}
uint48_t::uint48_t(std::uint8_t *bytes)
{
    memcpy(bytes_, bytes, sizeof(bytes_));
}
uint48_t::uint48_t(const uint48_t &val)
{
    memcpy(bytes_, val.bytes_, sizeof(bytes_));
}
uint48_t &uint48_t::operator =(const uint48_t &val)
{
    memcpy(bytes_, val.bytes_, sizeof(bytes_));
    return *this;
}
uint48_t &uint48_t::operator =(std::uint64_t val)
{
    memcpy(bytes_, &val, sizeof(bytes_));
    return *this;
}
std::uint64_t uint48_t::to_uint64() const
{
    return bytes_[0] | ((uint64_t)bytes_[1]) << 16 | ((uint64_t)bytes_[2]) << 32;
}
uint48_t uint48_t::operator +(uint64_t val)
{
    *this = to_uint64() + val;
    return *this;
}
uint48_t uint48_t::operator -(uint64_t val)
{
    *this = to_uint64() - val;
    return *this;
}
