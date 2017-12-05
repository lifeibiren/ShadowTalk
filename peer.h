#ifndef PEER_H
#define PEER_H

#include <string>

#include "key.h"

class peer
{
public:
    peer();
    std::string address() const;
    unsigned int port() const;
private:
    std::string name_;
    std::string address_;
    unsigned int port_;
    key pub_key_;
};

#endif // PEER_H
