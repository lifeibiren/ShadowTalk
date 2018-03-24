#include "aes_128.h"

namespace sml
{
aes_128::aes_128(sptr_bytes &key)
    : cfbEncryption(key_, CryptoPP::AES::DEFAULT_KEYLENGTH, iv_)
    , cfbDecryption(key_, CryptoPP::AES::DEFAULT_KEYLENGTH, iv_)
{
    if (key == nullptr)
    {
        CryptoPP::AutoSeededRandomPool rnd;
        rnd.GenerateBlock(key_, key_length_);
    }
    else
    {
        set_key(key);
    }
}
void aes_128::set_key(sptr_bytes key)
{
    if (key->size() != key_length_)
    {
        throw invalid_data_error();
    }
    memcpy(key_, key->c_str(), key_length_);
}
sptr_bytes aes_128::encrypt(sptr_bytes data)
{
    CryptoPP::AutoSeededRandomPool rnd;
    rnd.GenerateBlock(iv_, block_size_);

    int cipher_length = data->size() + block_size_;
    shared_array<byte> cipher = shared_array<byte>(new byte[cipher_length]);
    memcpy(cipher.get(), iv_, block_size_);
    cfbEncryption.ProcessData(cipher.get() + block_size_, (const byte*)data->c_str(), data->size());
    return make_shared<std::string>((char*)cipher.get(), cipher_length);
}
sptr_bytes aes_128::decrypt(sptr_bytes data)
{
    int plaintext_length = data->size() - block_size_;
    if (plaintext_length <= 0)
    {
        throw invalid_data_error();
    }
    memcpy(iv_, data->c_str() , block_size_);
    shared_array<byte> cipher = shared_array<byte>(new byte[plaintext_length]);
    cfbDecryption.ProcessData(cipher.get(), (const byte*)data->c_str() + block_size_, plaintext_length);
    return make_shared<std::string>((char*)cipher.get(), plaintext_length);
}
}
