#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include "sml.h"

namespace sml
{
class encryptor
{
public:
    virtual void set_key(sptr_string key) = 0;
    virtual std::string encrypt(const std::string& data) = 0;
    virtual std::string decrypt(const std::string& data) = 0;
    virtual sptr_string encrypt(sptr_string data) = 0;
    virtual sptr_string decrypt(sptr_string data) = 0;
};
}

#endif // ENCRYPTOR_H
