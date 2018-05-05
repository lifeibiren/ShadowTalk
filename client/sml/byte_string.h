#ifndef BYTE_STRING_H
#define BYTE_STRING_H

#include "config.h"

/**
 * @brief The byte_string class is used to handle network endian byte strings.
 * Internal representations in the class should all be in big endian
 * while it treats the endian of all methods' parameters as what it should be
 * depeneding on the architecture.
 */
namespace sml
{
class byte_string
{
public:
    enum class whence_type : std::uint8_t
    {
        seek_set = 0,
        seek_cur = 1,
        seek_end = 2
    };
    struct seek_out_of_range : exception_base
    {};

    byte_string()
        : buf_(nullptr)
        , data_len_(0)
        , buf_size_(0)
        , offset_(0)
    {}
    byte_string(int init_buf_size);
    byte_string(std::string& val);
    byte_string(const byte_string& val);
    byte_string(const void* buf_, int len);
    byte_string sub_byte_string(int start, int len);
    virtual ~byte_string();

    const char* c_array() const;
    std::string to_std_string() const;
    uint64_t can_write() const;
    uint64_t write(const void* buf, uint64_t n);
    uint64_t can_read() const;
    uint64_t read(void* buf, uint64_t n);
    uint64_t seek(whence_type whence, int64_t offset);
    void truncate(int n);

    uint64_t append(void* buf, int n);
    uint64_t append(const byte_string& val);

    template <typename T> uint64_t append(T val)
    {
        boost::endian::endian_buffer<boost::endian::order::big, T, sizeof(T) * 8> buffer(val);
        BOOST_STATIC_ASSERT(sizeof(buffer) == sizeof(T));
        append(&buffer, sizeof(T));
    }

    char& operator[](int index);
    uint64_t size() const;
    uint64_t capacity() const;
    byte_string& operator=(const byte_string& val);
    bool operator==(const byte_string& val) const;
    bool operator!=(const byte_string& val) const;
    bool operator<(const byte_string& val) const;
    bool operator>(const byte_string& val) const;
    bool operator<=(const byte_string& val) const;
    bool operator>=(const byte_string& val) const;
    byte_string& operator+(const byte_string& val);

    template <typename T> byte_string& operator<<(const T& val)
    {
        if (write(&val, sizeof(T)) != sizeof(T))
        {
            throw io_error();
        }
        return *this;
    }

    template <typename T> byte_string& operator>>(T& val)
    {
        if (read(&val, sizeof(T)) != sizeof(T))
        {
            throw io_error();
        }
        return *this;
    }

private:
    size_t gurantee_space(size_t size);
    size_t _change_buffer_size_to(size_t size);
    size_t adjust_buffer_size(size_t need);

    void might_update_data_len();

    const int ALIGNED_SIZE = 16;
    std::unique_ptr<char[]> buf_;
    int data_len_;
    int buf_size_;
    int offset_;
};
}

#endif // BYTE_STRING_H
