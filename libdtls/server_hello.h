#ifndef SERVER_HELLO_H
#define SERVER_HELLO_H

#include "dtls.h"

namespace dtls
{
class server_hello : public body
{
public:
    server_hello();

private:
    protocol_version server_version_;
    random random_;
    session_id session_id_;
    cipher_suite cipher_suite_;
    compression_method compression_method_;
    //    extension extensions_;
};
} // namespace dtls

#endif // SERVER_HELLO_H
