#ifndef KEY_H
#define KEY_H

#include <string>

class key
{
public:
    key();
    key(std::string key_val);
    const std::string &get_key_val() const;
    void set_key_val(const std::string &key_val);
private:
    std::string key_val_;
};

#endif // KEY_H
