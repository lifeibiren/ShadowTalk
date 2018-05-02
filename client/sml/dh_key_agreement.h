#ifndef DH_H
#define DH_H

#include "config.h"

#include <cryptopp/integer.h>
#include <cryptopp/dh.h>
#include <cryptopp/dh2.h>
#include <cryptopp/osrng.h>
#include <cryptopp/nbtheory.h>
#include <cryptopp/secblock.h>
#include <cryptopp/filters.h>

namespace sml{
class dh_key_agreement
{
public:
    dh_key_agreement();

    void generate_static_key_pairs();
    void set_ephemeral_key_pairs();
    void set_static_key_pairs(const std::string &static_priv_key_bytes, const std::string &static_pub_key_bytes);
    void set_ephemeral_key_pairs(const std::string &eph_priv_key_bytes, const std::string &eph_pub_key_bytes);
    void set_peer_pub_key_pairs(const std::string &static_pub_key_bytes, const std::string &eph_pub_key_bytes);
    shared_ptr<std::string> generate_shared_key();
private:
    const static CryptoPP::Integer ref_p, ref_q, ref_g;
    CryptoPP::AutoSeededRandomPool rnd;
    CryptoPP::Integer p, q, g;
    std::unique_ptr<CryptoPP::DH> dh;
    std::unique_ptr<CryptoPP::DH2> dh2;
    std::unique_ptr<CryptoPP::SecByteBlock> spriv, spub, epriv, epub, peer_spub, peer_epub, shared;
};
}

#endif // DH_H
