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
    std::vector<opaque> cookie_;
    std::vector<cipher_suite> cipher_suites_;
    std::vector<compression_method> compression_methods;
};
} // namespace dtls
#endif // CLIENT_HELLO_H
