#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include "sml.h"

namespace sml {
class encryptor
{
public:
    virtual sptr_string encrypt(sptr_string data) = 0;
    virtual sptr_string decrypt(sptr_string data) = 0;
};
} // namespace sml

#endif // ENCRYPTOR_H
