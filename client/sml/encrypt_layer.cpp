#include "encrypt_layer.h"
#include "aes_128.h"
namespace sml
{
encrypt_layer::encrypt_layer(algorithm algo, sptr_string key)
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

std::string encrypt_layer::encrypt(const std::string& data)
{
    return encryptor_->encrypt(data);
}

std::string encrypt_layer::decrypt(const std::string& data)
{
    return encryptor_->decrypt(data);
}

sptr_string encrypt_layer::encrypt(sptr_string data)
{
    return encryptor_->encrypt(data);
}
sptr_string encrypt_layer::decrypt(sptr_string data)
{
    return encryptor_->decrypt(data);
}
} // namespace sml
