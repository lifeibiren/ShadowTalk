#ifndef VL_VECTOR_H
#define VL_VECTOR_H

#include <vector>
#include "dtls.h"
#include "byte_string.h"

template <typename T, std::size_t low, std::size_t high>
class vl_vector
{
public:
    vl_vector()
    {

    }
    void from_bytes(const byte_string &bytes)
    {
    }
    byte_string to_bytes();
    T &operator[] (std::size_t);
private:
    std::size_t actual_len_;
    std::vector<T> elem_;
};

#endif // VL_VECTOR_H
