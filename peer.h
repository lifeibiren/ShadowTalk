#ifndef PEER_H
#define PEER_H

#include <string>
#include <ctime>
#include "key.h"

namespace shadowtalk {
class peer
{
public:
    peer();
    peer(const std::string &address, unsigned int port);
    std::string address() const;
    unsigned int port() const;
    bool dead() const;
    std::string to_string() const;

    bool operator=(const peer &r_peer);
    bool operator==(const peer &r_peer) const;
    bool operator!=(const peer &r_peer) const;
    bool operator<(const peer &r_peer) const;
    bool operator>(const peer &r_peer) const;
private:
    const int timeout = 30;
    std::string name_;
    std::string address_;
    unsigned int port_;
    key pub_key_;
    time_t last_beat_;
};
} //namespace shadowtalk

#endif // PEER_H
