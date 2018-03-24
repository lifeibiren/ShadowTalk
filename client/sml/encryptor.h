#ifndef ENCRYPTOR_H
#define ENCRYPTOR_H

#include "sml.h"

namespace sml
{
class encryptor
{
public:
    virtual sptr_bytes encrypt(sptr_bytes data) = 0;
    virtual sptr_bytes decrypt(sptr_bytes data) = 0;
};
}

#endif // ENCRYPTOR_H
