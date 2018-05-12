#include "byte_string.h"
#include "utils.h"

namespace sml
{
byte_string::byte_string(int init_buf_size)
    : byte_string()
{
    _change_buffer_size_to(init_buf_size);
}

byte_string::byte_string(std::string& val)
    : byte_string(val.size())
{
    data_len_ = val.size();
    gurantee_space(data_len_);
    memcpy(buf_.get(), val.c_str(), data_len_);
}

byte_string::byte_string(const byte_string& val)
    : byte_string(val.buf_size_)
{
    data_len_ = val.data_len_;
    memcpy(buf_.get(), val.buf_.get(), data_len_);
}

byte_string::byte_string(const void* buf, int len)
    : byte_string(len)
{
    data_len_ = len;
    memcpy(buf_.get(), buf, data_len_);
}

byte_string::~byte_string() {}

const char* byte_string::c_array() const
{
    return buf_.get();
}

std::string byte_string::to_std_string() const
{
    return std::string(buf_.get(), data_len_);
}

byte_string byte_string::sub_byte_string(int start, int len)
{
    return byte_string(buf_.get() + start, len);
}

uint64_t byte_string::can_write() const
{
    assert(buf_size_ >= offset_);
    return buf_size_ - offset_;
}

uint64_t byte_string::write(const void* buf, uint64_t n)
{
    if (buf == NULL || n == 0)
    {
        return 0;
    }
    (void)gurantee_space(offset_ + n);
    n = can_write() >= n ? n : can_write();
    bcopy(buf, buf_.get() + offset_, n);
    offset_ += n;
    might_update_data_len();
    return n;
}

uint64_t byte_string::can_read() const
{
    return data_len_ - offset_;
}

uint64_t byte_string::read(void* buf, uint64_t n)
{
    if (buf == NULL || n == 0)
    {
        return 0;
    }
    n = can_read() >= n ? n : can_read();
    bcopy(buf_.get() + offset_, (uint8_t*)buf, n);
    offset_ += n;
    return n;
}

uint64_t byte_string::seek(whence_type whence, int64_t offset)
{
    switch (whence)
    {
        case whence_type::seek_cur:
            offset_ += offset;
            break;
        case whence_type::seek_end:
            offset_ = data_len_ + offset;
            break;
        case whence_type::seek_set:
            offset_ = offset;
            break;
        default:
            return -1;
            break;
    }
    if (offset_ > data_len_)
    {
        throw seek_out_of_range();
    }
    return offset_;
}

uint64_t byte_string::append(void* buf, int n)
{
    (void)seek(whence_type::seek_end, 0);
    return write(buf, n);
}

uint64_t byte_string::append(const byte_string& val)
{
    return append(val.buf_.get(), val.data_len_);
}

char& byte_string::operator[](int index)
{
    if (index < data_len_ && index >= 0)
    {
        return buf_[index];
    }
    else
    {
        throw access_violation();
    }
}

uint64_t byte_string::size() const
{
    return data_len_;
}

uint64_t byte_string::capacity() const
{
    return buf_size_;
}

byte_string& byte_string::operator=(const byte_string& val)
{
    _change_buffer_size_to(val.buf_size_);
    data_len_ = val.data_len_;
    memcpy(buf_.get(), val.buf_.get(), val.data_len_);
    return *this;
}

bool byte_string::operator==(const byte_string& val) const
{
    if (data_len_ == val.data_len_)
    {
        return memcmp(buf_.get(), val.buf_.get(), data_len_) == 0;
    }
    else
    {
        return false;
    }
}

bool byte_string::operator!=(const byte_string& val) const
{
    return !(*this == val);
}

bool byte_string::operator<(const byte_string& val) const
{
    int min = utils::min(data_len_, val.data_len_);
    return memcmp(buf_.get(), val.buf_.get(), min) < 0;
}

bool byte_string::operator>(const byte_string& val) const
{
    return !(*this < val) && (*this != val);
}

bool byte_string::operator<=(const byte_string& val) const
{
    return !(*this > val);
}

bool byte_string::operator>=(const byte_string& val) const
{
    return !(*this < val);
}

byte_string& byte_string::operator+(const byte_string& val)
{
    append(val);
    return *this;
}

size_t byte_string::_change_buffer_size_to(size_t size)
{
    size = size >= 1 ? ((size - 1) & ~(ALIGNED_SIZE - 1)) + ALIGNED_SIZE : 0;
    if (size == 0)
    {
        buf_ = NULL;
        buf_size_ = 0;
        return 0;
    }
    std::unique_ptr<char[]> new_buf(new char[size]);
    if (!new_buf)
    {
        // failed to new
        return buf_size_;
    }
    if (buf_)
    {
        int min_size = utils::min(size, data_len_);
        memcpy(new_buf.get(), buf_.get(), min_size);
    }
    buf_ = boost::move(new_buf);
    buf_size_ = size;
    return buf_size_;
}

size_t byte_string::adjust_buffer_size(size_t need)
{
    int dst = 1;
    while (dst < need)
    {
        dst <<= 1;
        if (dst == 0)
        {
            return buf_size_;
        }
    }
    return _change_buffer_size_to(dst);
}

size_t byte_string::gurantee_space(size_t size)
{
    if (size > buf_size_)
    {
        return adjust_buffer_size(size);
    }
    return buf_size_;
}

void byte_string::might_update_data_len()
{
    data_len_ = offset_ > data_len_ ? offset_ : data_len_;
}

template <> byte_string& byte_string::operator<<(const std::string& val)
{
    if (write(val.c_str(), val.size()) != val.size())
    {
        throw io_error();
    }
    return *this;
}

template <> byte_string& byte_string::operator>>(std::string& val)
{
    uint64_t length = can_read();
    std::unique_ptr<char[]> buf(new char[length]);
    if (read(buf.get(), length) != length)
    {
        throw io_error();
    }
    val = std::string(buf.get(), length);
    return *this;
}
}
