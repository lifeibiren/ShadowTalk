#include "encrypt_layer.h"
#include "aes_128.h"
namespace sml
{
encrypt_layer::encrypt_layer(algorithm algo, sptr_bytes key)
{
    switch (algo)
    {
        case algorithm::AES_128:
            encryptor_ = shared_ptr<aes_128>(new aes_128(key));
            break;
        default:
            break;
    }
}
sptr_bytes encrypt_layer::encrypt(sptr_bytes data)
{
    return encryptor_->encrypt(data);
}
sptr_bytes encrypt_layer::decrypt(sptr_bytes data)
{
    return encryptor_->decrypt(data);
}
} // namespace sml
