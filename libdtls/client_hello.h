#ifndef CLIENT_HELLO_H
#define CLIENT_HELLO_H

#include "dtls.h"

namespace dtls {
class client_hello : public body
{
public:
    client_hello();

private:
    protocol_version protocol_version_;
    random random_;
    session_id session_id_;
    vl_vector<opaque, 0, utils::pow<2, 8>() - 1> cookie_;
    vl_vector<cipher_suite, 2, utils::pow<2, 16>() - 1> cipher_suites_;
    vl_vector<compression_method, 1, utils::pow<2, 8>() - 1> compression_methods;

};
} // namespace dtls
#endif // CLIENT_HELLO_H
