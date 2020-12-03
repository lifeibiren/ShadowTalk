#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "config.h"

#include <yaml-cpp/yaml.h>

namespace sml {
class configuration {
public:
    typedef std::map<std::string, std::string> trusted_peer_key_map_type;

    configuration(const sml::address &server, const std::string &id,
        const std::string &private_key, const std::string &public_key,
        uint16_t port, uint32_t timeout, uint32_t max_retries,
        const trusted_peer_key_map_type &trusted_peer_key_map);
    configuration(const YAML::Node &node);

    const address &server() const;
    const std::string &id() const;
    const std::string &public_key() const;
    const std::string &private_key() const;
    uint16_t port() const;
    uint32_t timeout() const;
    uint32_t max_retries() const;

    const trusted_peer_key_map_type &trusted_peer_key_map() const;
    void add_trusted_peer_public_key(
        const std::string &id, const std::string &public_key);
    const std::string get_peer_public_key_by_id(const std::string &id) const;

private:
    address server_;
    std::string id_;
    std::string pubkey_;
    std::string privkey_;
    uint16_t port_;
    uint32_t timeout_;
    uint32_t max_retries_;

    trusted_peer_key_map_type trusted_peer_key_map_;
};
}

#endif // CONFIGURATION_H
