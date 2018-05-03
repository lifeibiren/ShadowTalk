#ifndef AES_128_H
#define AES_128_H

#include "encryptor.h"
#include "sml.h"
#include <cryptopp/aes.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>

namespace sml
{
class encryptor;
class aes_128 : public encryptor
{
public:
    aes_128(sptr_string& key);
    void set_key(sptr_string key);

    std::string encrypt(const std::string& data);
    std::string decrypt(const std::string& data);
    sptr_string encrypt(sptr_string data);
    sptr_string decrypt(sptr_string data);

private:
    const static size_t block_size_ = CryptoPP::AES::BLOCKSIZE;
    const static size_t key_length_ = CryptoPP::AES::DEFAULT_KEYLENGTH;
    uint8_t iv_[block_size_];
    uint8_t key_[key_length_];
    CryptoPP::AutoSeededRandomPool rnd;
};
}

#endif // AES_128_H
