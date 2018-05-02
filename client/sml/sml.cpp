#include "sml.h"

namespace sml
{
logger log;
ring input_ring, output_ring;

service::service(asio::io_context &io_context, uint16_t port)
    : io_context_(io_context)
    , port_(port)
    , udp_layer_(io_context_, port)
{
}

void service::start()
{
    thread t(bind(&asio::io_context::run, &io_context_));
//    io_context_.run();
}

shared_ptr<peer> service::create_peer(const address& addr)
{
    return make_shared<peer>(io_context_, udp_layer_, addr);
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

shared_ptr<message> service::query()
{
    return output_ring.get();
}

void service::post(shared_ptr<message> msg)
{
    input_ring.put(msg);
    asio::post(io_context_, bind(&service::msg_handler, this));
}

void service::msg_handler()
{
    shared_ptr<message> ptr;
    while ((ptr = input_ring.get()) != nullptr)
    {
        control_message *ctl_ptr = dynamic_cast<control_message *>(ptr.get());
        ctl_ptr->operator ()(udp_layer_);
    }
}

void service::new_peer_handler(shared_ptr<std::string> msg, shared_ptr<address> addr)
{
    shared_ptr<peer> new_peer = make_shared<peer>(io_context_, udp_layer_, *addr);
    new_peer->feed(msg);

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
