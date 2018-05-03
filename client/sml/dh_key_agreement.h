#ifndef DH_H
#define DH_H

#include "config.h"

#include <cryptopp/dh.h>
#include <cryptopp/dh2.h>
#include <cryptopp/filters.h>
#include <cryptopp/integer.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/osrng.h>
#include <cryptopp/secblock.h>

namespace sml
{
class dh_key_agreement
{
public:
    dh_key_agreement();

    void generate_static_key_pairs();
    void generate_ephemeral_key_pairs();
    void set_static_key_pairs(const std::string& static_priv_key_bytes, const std::string& static_pub_key_bytes);
    void set_ephemeral_key_pairs(const std::string& eph_priv_key_bytes, const std::string& eph_pub_key_bytes);
    void set_peer_pub_key_pairs(const std::string& static_pub_key_bytes, const std::string& eph_pub_key_bytes);
    void generate_shared_key();
    std::string generate_content_encryption_key_and_cma();
    void feed_peer_content_encryption_key_and_cma(const std::string& cek);
    std::string content_encryption_key(); // CEK used by AES
    std::string ephemeral_priv_key() const;
    std::string ephemeral_pub_key() const;
    std::string shared_key() const;

private:
    const static CryptoPP::Integer ref_p, ref_q, ref_g;
    CryptoPP::AutoSeededRandomPool rnd;
    CryptoPP::Integer p, q, g;
    std::unique_ptr<CryptoPP::DH> dh;
    std::unique_ptr<CryptoPP::DH2> dh2;
    std::unique_ptr<CryptoPP::SecByteBlock> spriv, spub, epriv, epub, peer_spub, peer_epub;
    std::unique_ptr<CryptoPP::SecByteBlock> shared; // KEK
    std::unique_ptr<CryptoPP::SecByteBlock> cek_; // content encryption key(CEK for AES)
    std::unique_ptr<CryptoPP::SecByteBlock> cek_cmac_; // CMAC for CEK
};
}

#endif // DH_H
