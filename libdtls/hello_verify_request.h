#ifndef HELLO_VERIFY_REQUEST_H
#define HELLO_VERIFY_REQUEST_H

#include "dtls.h"

class hello_verify_request
{
public:
    hello_verify_request();

private:
    protocol_version server_version_;
    vl_vector<opaque, 0, utils::pow<2, 8>() - 1> cookie;
};

#endif // HELLO_VERIFY_REQUEST_H
