#ifndef CONNECTION_STATE_H
#define CONNECTION_STATE_H

#include <cstdint>
class session
{
public:
    session();

private:
    //    enum { server, client } connection_end;

    //    enum { tls_prf_sha256 } PRFAlgorithm;

    //    enum { null, rc4, _3des, aes }
    //      BulkCipherAlgorithm;

    //    enum { stream, block, aead } CipherType;

    //    enum { null, hmac_md5, hmac_sha1, hmac_sha256,
    //         hmac_sha384, hmac_sha512} MACAlgorithm;

    //    enum { null } CompressionMethod;

    //    using namespace std;

    //    struct {
    //        connection_end         entity;
    //        PRFAlgorithm           prf_algorithm;
    //        BulkCipherAlgorithm    bulk_cipher_algorithm;
    //        CipherType             cipher_type;
    //        uint8                  enc_key_length;
    //        uint8                  block_length;
    //        uint8                  fixed_iv_length;
    //        uint8                  record_iv_length;
    //        MACAlgorithm           mac_algorithm;
    //        uint8                  mac_length;
    //        uint8                  mac_key_length;
    //        CompressionMethod      compression_algorithm;
    //        opaque                 master_secret[48];
    //        opaque                 client_random[32];
    //        opaque                 server_random[32];
    //    } SecurityParameters;
};

#endif // CONNECTION_STATE_H
