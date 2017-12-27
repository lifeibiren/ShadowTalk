#ifndef BYTE_STRING_H
#define BYTE_STRING_H

#include <cinttypes>
#include <string>
class byte_string
{
public:
    byte_string() : buf_(NULL), data_len_(0), buf_size_(0) {}
    byte_string(std::size_t init_buf_size);
    byte_string(std::string &val);
    byte_string(const byte_string &val);
    byte_string(const char *buf_, std::size_t len);
    byte_string sub_byte_string(std::size_t start, std::size_t len);
    virtual ~byte_string();

    void push_back(char &val);
    void pop_back();
    const char *c_array() const;
    char &operator [](int index);
    byte_string &operator =(const byte_string &val);
    bool operator ==(const byte_string &val);
    bool operator !=(const byte_string &val);
    bool operator <(const byte_string &val);
    bool operator >(const byte_string &val);
    bool operator <=(const byte_string &val);
    bool operator >=(const byte_string &val);

    void switch_endian();
private:
    void adjust_buf(std::size_t size);
    char *buf_;
    std::size_t data_len_;
    std::size_t buf_size_;
};

#endif // BYTE_STRING_H
