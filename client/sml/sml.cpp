#include "sml.h"

namespace sml
{
asio::io_context io_context;

service::service(uint16_t port) :
    port_(port), udp_layer_(port)
{

}

shared_ptr<peer> service::create_peer(const address &addr)
{
    return make_shared<peer>(udp_layer_, addr);
}

void service::async_accept_peer(handler_type handler)
{

}
} // namespace sml
