#ifndef ENCRYPT_LAYER_H
#define ENCRYPT_LAYER_H


#include "sml.h"

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
    encrypt_layer(algorithm algo, sptr_bytes key);
    sptr_bytes encrypt(sptr_bytes data);
    sptr_bytes decrypt(sptr_bytes data);
private:
    shared_ptr<encryptor> encryptor_;
};
} // namespace sml

#endif // ENCRYPT_LAYER_H
