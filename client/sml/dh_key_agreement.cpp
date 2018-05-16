#include "dh_key_agreement.h"

namespace sml
{
// RFC 5114, 1024-bit MODP Group with 160-bit Prime Order Subgroup
// http://tools.ietf.org/html/rfc5114#section-2.1
const CryptoPP::Integer dh_key_agreement::ref_p("0xB10B8F96A080E01DDE92DE5EAE5D54EC52C99FBCFB06A3C6"
                                                "9A6A9DCA52D23B616073E28675A23D189838EF1E2EE652C0"
                                                "13ECB4AEA906112324975C3CD49B83BFACCBDD7D90C4BD70"
                                                "98488E9C219A73724EFFD6FAE5644738FAA31A4FF55BCCC0"
                                                "A151AF5F0DC8B4BD45BF37DF365C1A65E68CFDA76D4DA708"
                                                "DF1FB2BC2E4A4371");

const CryptoPP::Integer dh_key_agreement::ref_g("0xA4D1CBD5C3FD34126765A442EFB99905F8104DD258AC507F"
                                                "D6406CFF14266D31266FEA1E5C41564B777E690F5504F213"
                                                "160217B4B01B886A5E91547F9E2749F4D7FBD7D3B9A92EE1"
                                                "909D0D2263F80A76A6A24C087A091F531DBF0A0169B6A28A"
                                                "D662A4D18E73AFA32D779D5918D08BC8858F4DCEF97C2A24"
                                                "855E6EEB22B3B2E5");

const CryptoPP::Integer dh_key_agreement::ref_q("0xF518AA8781A8DF278ABA4E7D64B7CB9D49462353");

// Schnorr Group primes are of the form p = rq + 1, p and q prime. They
// provide a subgroup order. In the case of 1024-bit MODP Group, the
// security level is 80 bits (based on the 160-bit prime order subgroup).

// For a compare/contrast of using the maximum security level, see
// dh-unified.zip. Also see http://www.cryptopp.com/wiki/Diffie-Hellman
// and http://www.cryptopp.com/wiki/Security_level .

inline std::unique_ptr<CryptoPP::SecByteBlock> convert_bytes_to_SecByteBlock(const std::string bytes)
{
    return boost::make_unique<CryptoPP::SecByteBlock>((const byte *)bytes.c_str(), bytes.size());
}

dh_key_agreement::dh_key_agreement()
    : dh(boost::make_unique<CryptoPP::DH>())
{
    dh->AccessGroupParameters().Initialize(ref_p, ref_q, ref_g);

    if (!dh->GetGroupParameters().ValidateGroup(rnd, 3))
    {
        throw std::runtime_error("Failed to validate prime and generator");
    }

    p = dh->GetGroupParameters().GetModulus();
    q = dh->GetGroupParameters().GetSubgroupOrder();
    g = dh->GetGroupParameters().GetGenerator();

    // http://groups.google.com/group/sci.crypt/browse_thread/thread/7dc7eeb04a09f0ce
    CryptoPP::Integer v = CryptoPP::ModularExponentiation(g, q, p);
    if (v != CryptoPP::Integer::One())
    {
        throw std::runtime_error("Failed to verify order of the subgroup");
    }

    dh2 = make_unique<CryptoPP::DH2>(*dh);
}

void dh_key_agreement::generate_static_key_pairs()
{
    spriv = make_unique<CryptoPP::SecByteBlock>(dh2->StaticPrivateKeyLength());
    spub = make_unique<CryptoPP::SecByteBlock>(dh2->StaticPublicKeyLength());
    dh2->GenerateStaticKeyPair(rnd, *spriv, *spub);
}

void dh_key_agreement::generate_ephemeral_key_pairs()
{
    epriv = make_unique<CryptoPP::SecByteBlock>(dh2->EphemeralPrivateKeyLength());
    epub = make_unique<CryptoPP::SecByteBlock>(dh2->EphemeralPublicKeyLength());
    dh2->GenerateEphemeralKeyPair(rnd, *epriv, *epub);
}

void dh_key_agreement::set_static_key_pairs(
    const std::string &static_priv_key_bytes, const std::string &static_pub_key_bytes)
{
    spriv = convert_bytes_to_SecByteBlock(static_priv_key_bytes);
    spub = convert_bytes_to_SecByteBlock(static_pub_key_bytes);

//    spriv = boost::make_unique<CryptoPP::SecByteBlock>(
//        (const byte *)static_priv_key_bytes.c_str(), static_priv_key_bytes.size());
//    spub = boost::make_unique<CryptoPP::SecByteBlock>(
//        (const byte *)static_pub_key_bytes.c_str(), static_pub_key_bytes.size());
}

void dh_key_agreement::set_ephemeral_key_pairs(
    const std::string &eph_priv_key_bytes, const std::string &eph_pub_key_bytes)
{
    epriv = convert_bytes_to_SecByteBlock(eph_priv_key_bytes);
    epub = convert_bytes_to_SecByteBlock(eph_pub_key_bytes);
//    epriv = boost::make_unique<CryptoPP::SecByteBlock>(
//        (const byte *)eph_priv_key_bytes.c_str(), eph_priv_key_bytes.size());
//    epub
//        = boost::make_unique<CryptoPP::SecByteBlock>((const byte *)eph_pub_key_bytes.c_str(), eph_pub_key_bytes.size());
}

void dh_key_agreement::set_peer_pub_key_pairs(
    const std::string &static_pub_key_bytes, const std::string &eph_pub_key_bytes)
{
    peer_spub = convert_bytes_to_SecByteBlock(static_pub_key_bytes);
    peer_epub = convert_bytes_to_SecByteBlock(eph_pub_key_bytes);
//    peer_spub = boost::make_unique<CryptoPP::SecByteBlock>(
//        (const byte *)static_pub_key_bytes.c_str(), static_pub_key_bytes.size());
//    peer_epub
//        = boost::make_unique<CryptoPP::SecByteBlock>((const byte *)eph_pub_key_bytes.c_str(), eph_pub_key_bytes.size());
}

void dh_key_agreement::generate_shared_key()
{
    shared = make_unique<CryptoPP::SecByteBlock>(dh2->AgreedValueLength());

    if (!dh2->Agree(*shared, *spriv, *epriv, *peer_spub, *peer_epub))
    {
        throw std::runtime_error("Failed to reach shared secret");
    }

    CryptoPP::Integer a; // b;
    a.Decode(shared->BytePtr(), shared->SizeInBytes());
    std::cout << "Shared secret (A): " << std::hex << a << std::endl;
}

std::string dh_key_agreement::content_encryption_key()
{
    return std::string((const char *)cek_->BytePtr(), cek_->size());
}

std::string dh_key_agreement::ephemeral_priv_key() const
{
    return std::string((const char *)epriv->BytePtr(), epriv->size());
}

std::string dh_key_agreement::ephemeral_pub_key() const
{
    return std::string((const char *)epub->BytePtr(), epub->size());
}

std::string dh_key_agreement::shared_key() const
{
    return std::string((const char *)shared->BytePtr(), shared->size());
}
}
