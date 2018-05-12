#include "sml.h"

namespace sml
{
logger log;
ring input_ring, output_ring;

service::service(asio::io_context &io_context, const configuration &conf)
    : io_context_(io_context)
    , conf_(conf)
    , udp_layer_(*this)
{}

shared_ptr<message> service::query()
{
    return output_ring.get();
}

void service::post(shared_ptr<message> msg)
{
    input_ring.put(msg);
    asio::post(io_context_, bind(&service::msg_handler, this));
}

asio::io_context &service::io_context()
{
    return io_context_;
}

configuration &service::conf()
{
    return conf_;
}

void service::msg_handler()
{
    shared_ptr<message> ptr;
    while ((ptr = input_ring.get()) != nullptr)
    {
        control_message *ctl_ptr = dynamic_cast<control_message *>(ptr.get());
        ctl_ptr->operator()(udp_layer_);
    }
}
} // namespace sml
