#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "config.h"

namespace sml
{
class configuration
{
public:
    configuration(const std::string &id, const std::string &private_key, const std::string &public_key, uint16_t port,
        uint32_t timeout, uint32_t max_retries);

    const std::string &id() const;
    const std::string &public_key() const;
    const std::string &private_key() const;
    uint16_t port() const;
    uint32_t timeout() const;
    uint32_t max_retries() const;

    void add_trusted_peer_public_key(const std::string &id, const std::string &public_key);
    const std::string get_peer_public_key_by_id(const std::string &id) const;

private:
    std::string id_;
    std::string pubkey_;
    std::string privkey_;
    uint16_t port_;
    uint32_t timeout_;
    uint32_t max_retries_;

    typedef boost::unordered_map<std::string, std::string> trusted_key_map_type;
    trusted_key_map_type trusted_key_map_;
};
}

#endif // CONFIGURATION_H
