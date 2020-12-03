#include "aes_128.h"

namespace sml {
aes_128::aes_128(sptr_string &key) {
    if (key == nullptr) {
        CryptoPP::AutoSeededRandomPool rnd;
        rnd.GenerateBlock(key_, key_length_);
    } else {
        set_key(key);
    }
}
void aes_128::set_key(sptr_string key) {
    if (key->size() < key_length_) {
        throw invalid_data_error();
    }

    memcpy(key_, key->c_str(), key_length_);
    for (int i = key_length_; i < key->size(); i++) {
        key_[i % key_length_] ^= (*key)[i];
    }
}

std::string aes_128::encrypt(const std::string &data) {
    rnd.GenerateBlock(iv_, block_size_);
    int cipher_length = data.size() + block_size_;
    std::unique_ptr<byte[]> cipher = make_unique<byte[]>(cipher_length);
    memcpy(cipher.get(), iv_, block_size_);
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(
        key_, CryptoPP::AES::DEFAULT_KEYLENGTH, iv_);
    cfbEncryption.ProcessData(
        cipher.get() + block_size_, (const byte *)data.c_str(), data.size());
    return std::string((char *)cipher.get(), cipher_length);
}

std::string aes_128::decrypt(const std::string &data) {
    int plaintext_length = data.size() - block_size_;
    if (plaintext_length <= 0) {
        throw invalid_data_error();
    }
    memcpy(iv_, data.c_str(), block_size_);
    std::unique_ptr<byte[]> cipher = make_unique<byte[]>(plaintext_length);
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption cfbDecryption(
        key_, CryptoPP::AES::DEFAULT_KEYLENGTH, iv_);
    cfbDecryption.ProcessData(cipher.get(),
        (const byte *)data.c_str() + block_size_, plaintext_length);
    return std::string((char *)cipher.get(), plaintext_length);
}

sptr_string aes_128::encrypt(sptr_string data) {
    return boost::make_shared<std::string>(encrypt(*data));
    //    rnd.GenerateBlock(iv_, block_size_);
    //    int cipher_length = data->size() + block_size_;
    //    shared_array<byte> cipher = shared_array<byte>(new
    //    byte[cipher_length]); memcpy(cipher.get(), iv_, block_size_);
    //    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption cfbEncryption(key_,
    //    CryptoPP::AES::DEFAULT_KEYLENGTH, iv_);
    //    cfbEncryption.ProcessData(cipher.get() + block_size_, (const
    //    byte*)data->c_str(), data->size()); return
    //    make_shared<std::string>((char*)cipher.get(), cipher_length);
}
sptr_string aes_128::decrypt(sptr_string data) {
    return boost::make_shared<std::string>(decrypt(*data));
    //    int plaintext_length = data->size() - block_size_;
    //    if (plaintext_length <= 0)
    //    {
    //        throw invalid_data_error();
    //    }
    //    memcpy(iv_, data->c_str(), block_size_);
    //    shared_array<byte> cipher = shared_array<byte>(new
    //    byte[plaintext_length]); CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption
    //    cfbDecryption(key_, CryptoPP::AES::DEFAULT_KEYLENGTH, iv_);
    //    cfbDecryption.ProcessData(cipher.get(), (const byte*)data->c_str() +
    //    block_size_, plaintext_length); return
    //    make_shared<std::string>((char*)cipher.get(), plaintext_length);
}
}
