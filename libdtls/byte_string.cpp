#include "byte_string.h"
#include <string.h>
#include "exception.h"
#include "compare.h"

byte_string::byte_string(int init_buf_size) :
    byte_string()
{
    change_buffer_size_to(init_buf_size);
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
byte_string::byte_string(const char *buf, int len) :
    byte_string(len)
{
    data_len_ = len;
    memcpy(buf_, buf, data_len_);
}
byte_string::~byte_string()
{
    delete buf_;
}
const char *byte_string::c_array() const
{
    return buf_;
}
std::string byte_string::to_std_string() const
{
    return std::string(buf_, data_len_);
}
byte_string byte_string::sub_byte_string(int start, int len)
{
    return byte_string(buf_ + start, len);
}
void byte_string::push_back(void *buf, int n, bool switch_endian)
{
    assert(buf != NULL || n == 0);
    gurantee_empty_space(n);
    memcpy(buf_, buf, n);
    if (switch_endian) {
        change_endian(buf_ + data_len_, n);
    }
    data_len_ += n;
}
void byte_string::push_back(const byte_string &val, bool switch_endian)
{
    push_back(val.buf_, val.data_len_, switch_endian);
}
void byte_string::pop_back(void *buf, int n, bool switch_endian)
{
    if (buf != NULL) {
        if (switch_endian) {
            change_endian(buf_ + data_len_ - n, n);
        }
        memcpy(buf, buf_ + data_len_ - n, n);
    }
    data_len_ -= n;
}
void byte_string::extract(void *buf, int start, int length, bool switch_endian)
{
    assert(start >= 0 && length >=0 && start + length < data_len_);
    memcpy(buf, buf_ + start, length);
    if (switch_endian) {
        change_endian((char *)buf, length);
    }
}
char &byte_string::operator [](int index)
{
    if (index < data_len_ && index >= 0) {
        return buf_[index];
    } else {
        throw access_violation();
    }
}
int byte_string::size() const
{
    return data_len_;
}
int byte_string::capacity() const
{
    return buf_size_;
}
byte_string &byte_string::operator =(const byte_string &val)
{
    change_buffer_size_to(val.buf_size_);
    data_len_ = val.data_len_;
    memcpy(buf_, val.buf_, val.data_len_);
}
bool byte_string::operator ==(const byte_string &val) const
{
    if (data_len_ == val.data_len_) {
        return memcmp(buf_, val.buf_, data_len_) == 0;
    } else {
        return false;
    }
}
bool byte_string::operator !=(const byte_string &val) const
{
    return !(*this == val);
}
bool byte_string::operator <(const byte_string &val) const
{
    int min = compare::min(data_len_, val.data_len_);
    return memcmp(buf_, val.buf_, min) < 0;
}
bool byte_string::operator >(const byte_string &val) const
{
    return !(*this < val) && (*this != val);
}
bool byte_string::operator <=(const byte_string &val) const
{
    return !(*this > val);
}
bool byte_string::operator >=(const byte_string &val) const
{
    return !(*this < val);
}
byte_string &byte_string::operator +(const byte_string &val)
{
    push_back(val);
    return *this;
}
void byte_string::change_endian(char *buf, int len)
{
    char tmp;
    for (int i = 0, j = len - 1; i < j; i ++, j --) {
        tmp = buf[i];
        buf[i] = buf[j];
        buf[j] = tmp;
    }
}
void byte_string::change_buffer_size_to(int size)
{
    assert(size >= 0);
    if (size == 0) {
        return;
    }
    size = ((size - 1) & ~(ALIGNED_SIZE - 1)) + ALIGNED_SIZE;
    char *new_buf = new char[size];
    int min_size = compare::min(size, data_len_);
    memcpy(new_buf, buf_, min_size);
    delete buf_;
    buf_ = new_buf;
    buf_size_ = size;
}
void byte_string::adjust_buffer_size(int need)
{
    int dst = buf_size_;
    while ((dst <<= 1) < need);
    change_buffer_size_to(dst);
}
void byte_string::gurantee_empty_space(int size)
{
    if (data_len_ + size > buf_size_) {
        adjust_buffer_size(size);
    }
}
