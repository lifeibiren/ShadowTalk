#ifndef BYTE_STRING_H
#define BYTE_STRING_H

#include <cinttypes>
#include <string>
class byte_string
{
public:
    byte_string() : buf_(NULL), data_len_(0), buf_size_(0) {}
    byte_string(int init_buf_size);
    byte_string(std::string &val);
    byte_string(const byte_string &val);
    byte_string(const char *buf_, int len);
    byte_string sub_byte_string(int start, int len);
    virtual ~byte_string();

    const char *c_array() const;
    std::string to_std_string() const;

    void push_back(void *buf, int n, bool switch_endian = false);
    void push_back(const byte_string &val, bool switch_endian = false);
    void pop_back(void *buf = NULL, int n = 1, bool switch_endian = false);
    void extract(void *buf, int start, int length, bool switch_endian = false);

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

private:
    void gurantee_empty_space(int size);
    void change_buffer_size_to(int size);
    void adjust_buffer_size(int need);
    void change_endian(char *buf, int len);

    const int ALIGNED_SIZE = 16;
    char *buf_;
    int data_len_;
    int buf_size_;
};

#endif // BYTE_STRING_H
