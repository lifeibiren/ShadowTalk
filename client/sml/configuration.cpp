#include "configuration.h"

namespace sml
{
configuration::configuration(const sml::address &server, const std::string &id, const std::string &private_key,
                             const std::string &public_key, uint16_t port, uint32_t timeout, uint32_t max_retries,
                             const trusted_peer_key_map_type& trusted_peer_key_map)
    : server_(server)
    , id_(id)
    , privkey_(private_key)
    , pubkey_(public_key)
    , port_(port)
    , timeout_(timeout)
    , max_retries_(max_retries)
    , trusted_peer_key_map_(trusted_peer_key_map)
{}

configuration::configuration(const YAML::Node &node)
{
    server_ = node["server"].as<address>();
    id_ = node["id"].as<std::string>();
    privkey_ = node["private_key"].as<std::string>();
    pubkey_ = node["public_key"].as<std::string>();
    port_ = node["port"].as<uint16_t>();
    timeout_ = node["timeout"].as<uint32_t>();
    max_retries_ = node["max_retries"].as<uint32_t>();
    trusted_peer_key_map_ = node["trusted_peers"].as<trusted_peer_key_map_type>();
}

const address &configuration::server() const
{
    return server_;
}

const std::string &configuration::id() const
{
    return id_;
}

const std::string &configuration::public_key() const
{
    return pubkey_;
}

const std::string &configuration::private_key() const
{
    return privkey_;
}

uint16_t configuration::port() const
{
    return port_;
}

uint32_t configuration::timeout() const
{
    return timeout_;
}

uint32_t configuration::max_retries() const
{
    return max_retries_;
}

const configuration::trusted_peer_key_map_type &configuration::trusted_peer_key_map() const
{
    return trusted_peer_key_map_;
}

void configuration::add_trusted_peer_public_key(const std::string &id, const std::string &public_key)
{
    trusted_peer_key_map_[id] = public_key;
}

const std::string configuration::get_peer_public_key_by_id(const std::string &id) const
{
    trusted_peer_key_map_type::const_iterator it = trusted_peer_key_map_.find(id);
    if (it != trusted_peer_key_map_.end())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}
}
