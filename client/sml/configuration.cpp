#include "configuration.h"

namespace sml {
configuration::configuration(const std::string &id, const std::string &private_key,
                             const std::string &public_key, uint16_t port,
                             uint32_t timeout, uint32_t max_retries)
    : id_(id), privkey_(private_key), pubkey_(public_key), port_(port),
      timeout_(timeout), max_retries_(max_retries)
{

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

void configuration::add_trusted_peer_public_key(const std::string& id, const std::string& public_key)
{
    trusted_key_map_[id] = public_key;
}

const std::string configuration::get_peer_public_key_by_id(const std::string &id) const
{
    trusted_key_map_type::const_iterator it = trusted_key_map_.find(id);
    if (it != trusted_key_map_.end())
    {
        return it->second;
    }
    else
    {
        return std::string();
    }
}
}
