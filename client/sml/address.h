#ifndef ADDRESS_H
#define ADDRESS_H

#include "config.h"

namespace sml
{
class address
{
public:
    address();
    address(const std::string &ip_addr, uint16_t port);
    address(const asio::ip::udp::endpoint &endpoint);
    address(const address &val);
    address(const std::string &string);
    const std::string &ip() const;
    void set_ip(const std::string &ip);
    uint16_t port() const;
    void set_port(uint16_t port);
    bool operator==(const address &val) const;

    size_t hash() const;
    std::string to_string() const;
private:
    std::string ip_addr_;
    uint16_t port_;
};
}

#include <yaml-cpp/yaml.h>
namespace YAML {
template<>
struct convert<sml::address> {
  static Node encode(const sml::address& rhs) {
    return Node(rhs.to_string());
  }

  static bool decode(const Node& node, sml::address& rhs) {
    if(!node.IsScalar()) {
      return false;
    }
    rhs = sml::address(node.as<std::string>());
    return true;
  }
};
}
#endif // ADDRESS_H
