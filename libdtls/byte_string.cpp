#include "byte_string.h"
#include <string.h>
byte_string::byte_string(std::size_t init_buf_size) :
    byte_string()
{
    adjust_buf(init_buf_size);
}
byte_string::byte_string(std::string &val) :
    byte_string(val.size())
{
    memcpy(buf_, val.c_str(), val.size());
}
byte_string::byte_string(const byte_string &val) :
    byte_string(val.buf_size_)
{
    data_len_ = val.data_len_;
    memcpy(buf_, val.buf_, data_len_);
}
byte_string::byte_string(const char *buf, std::size_t len) :
    byte_string(len)
{
    data_len_ = len;
    memcpy(buf_, buf, data_len_);
}
byte_string byte_string::sub_byte_string(std::size_t start, std::size_t len)
{
    return byte_string(buf_ + start, len);
}
void byte_string::adjust_buf(std::size_t size)
{
    char *new_buf = new char[size];
    int min_size = size > data_len_ ? data_len_ : size;
    memcpy(new_buf, buf_, min_size);
    delete buf_;
    buf_ = new_buf;
    buf_size_ = size;
}

void byte_string::switch_endian()
{
    char tmp;
    for (int i = 0, j = data_len_ - 1; i < j; i ++, j --) {
        tmp = buf_[i];
        buf_[i] = buf_[j];
        buf_[j] = tmp;
    }
}
