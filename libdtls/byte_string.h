#ifndef BYTE_STRING_H
#define BYTE_STRING_H

#include <cinttypes>
#include <string>
#include <boost/endian/endian.hpp>

/**
 * @brief The byte_string class is used to handle network endian byte strings.
 * Internal representations in the class should all be in big endian
 * while it treats the endian of all methods' parameters as what it should be
 * depeneding on the architecture.
 */
class byte_string
{
public:
    enum class whence_type : std::uint8_t {
        seek_set = 0,
        seek_cur = 1,
        seek_end = 2
    };
    byte_string() : convert_endian(true), buf_(NULL), data_len_(0), buf_size_(0), offset_(0) {}
    byte_string(int init_buf_size);
    byte_string(std::string &val);
    byte_string(const byte_string &val);
    byte_string(const void *buf_, int len);
    byte_string sub_byte_string(int start, int len);
    virtual ~byte_string();

    const char *c_array() const;
    std::string to_std_string() const;

    void write_at(void *buf, int n, int offset);
    void read_at(void *buf, int n, int offset);
    void write(void *buf, int n);
    void read(void *buf, int n);
    void truncate(int n);

    void push_back(void *buf, int n);
    void push_back(const byte_string &val);

    void pop_back(void *buf = NULL, int n = 1);
    void extract(void *buf, int start, int length);

    template<typename T>
    void push_back(T val)
    {
        boost::endian::endian_buffer<boost::endian::order::big, T, sizeof(T) * 8> buffer(val);
        BOOST_STATIC_ASSERT(sizeof(buffer) == sizeof(T));
        push_back(&buffer, sizeof(T));
    }

    char &operator [](int index);
    int size() const;
    int capacity() const;
    byte_string &operator =(const byte_string &val);
    bool operator ==(const byte_string &val) const;
    bool operator !=(const byte_string &val) const;
    bool operator <(const byte_string &val) const;
    bool operator >(const byte_string &val) const;
    bool operator <=(const byte_string &val) const;
    bool operator >=(const byte_string &val) const;
    byte_string &operator +(const byte_string &val);

    template<typename T>
    byte_string &operator <<(T val)
    {
        write(&val, sizeof(T));
        return *this;
    }
    template<typename T>
    byte_string &operator >>(T val)
    {
        read(&val, sizeof(T));
        return *this;
    }

    void rewind();
    void seek(whence_type whence, int offset);
private:
    void gurantee_space(int size);
    void change_buffer_size_to(int size);
    void adjust_buffer_size(int need);

    bool convert_endian;
    const int ALIGNED_SIZE = 16;
    char *buf_;
    int data_len_;
    int buf_size_;
    int offset_;
};

#endif // BYTE_STRING_H
