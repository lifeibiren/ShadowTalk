#ifndef ENCRYPT_LAYER_H
#define ENCRYPT_LAYER_H

#include "config.h"

namespace sml
{
class encryptor;
class encrypt_layer
{
public:
    enum class algorithm : int
    {
        AES_128
    };
    encrypt_layer(algorithm algo, sptr_string key);
    sptr_string encrypt(sptr_string data);
    sptr_string decrypt(sptr_string data);

private:
    shared_ptr<encryptor> encryptor_;
};
} // namespace sml

#endif // ENCRYPT_LAYER_H
