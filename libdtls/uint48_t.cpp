#include "uint48_t.h"
#include <string.h>
uint48_t::uint48_t() : bytes_{0, 0, 0}
{
}
uint48_t::uint48_t(uint64_t val)
{
    memcpy(bytes_, &val, sizeof(bytes_));
}
uint48_t::uint48_t(uint8_t *bytes)
{
    memcpy(bytes_, bytes, sizeof(bytes_));
}
uint48_t::uint48_t(const uint48_t &val)
{
    memcpy(bytes_, val.bytes_, sizeof(bytes_));
}
uint48_t &uint48_t::operator=(const uint48_t &val)
{
    memcpy(bytes_, val.bytes_, sizeof(bytes_));
    return *this;
}
uint48_t &uint48_t::operator=(const uint64_t val)
{
    memcpy(bytes_, &val, sizeof(bytes_));
    return *this;
}
uint64_t uint48_t::value() const
{
    return bytes_[0] | ((uint64_t)bytes_[1]) << 16 | ((uint64_t)bytes_[2]) << 32;
}
