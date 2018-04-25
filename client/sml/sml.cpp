#include "sml.h"

namespace sml
{
asio::io_context io_context;

service::service(uint16_t port)
    : port_(port)
    , udp_layer_(port)
{
    udp_layer_.register_handler(bind(&service::new_peer_handler, shared_from_this(), _1, _2));
}

shared_ptr<peer> service::create_peer(const address& addr)
{
    return make_shared<peer>(udp_layer_, addr);
}

void service::async_accept_peer(handler_type handler)
{
    if (!new_peer_list_.empty())
    {
        shared_ptr<peer> new_peer = new_peer_list_[0];
        new_peer_list_.erase(new_peer_list_.begin());
        handler(new_peer);
    }
    else
    {
        handler_list_.push_back(handler);
    }
}

void service::new_peer_handler(shared_ptr<std::string> msg, shared_ptr<address> addr)
{
    shared_ptr<peer> new_peer = make_shared<peer>(udp_layer_, *addr);
    new_peer->feed(msg, addr);

    if (!handler_list_.empty())
    {
        handler_type handler = handler_list_[0];
        handler_list_.erase(handler_list_.begin());
        handler(new_peer);
    }
    else
    {
        new_peer_list_.push_back(new_peer);
    }
}
} // namespace sml
