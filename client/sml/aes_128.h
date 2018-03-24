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
    aes_128(sptr_bytes &key);
    void set_key(sptr_bytes key);
    sptr_bytes encrypt(sptr_bytes data);
    sptr_bytes decrypt(sptr_bytes data);
private:
    const static size_t block_size_ = CryptoPP::AES::BLOCKSIZE;
    const static size_t key_length_ = CryptoPP::AES::DEFAULT_KEYLENGTH;
    uint8_t iv_[block_size_];
    uint8_t key_[key_length_];
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption;
};
}

#endif // AES_128_H
