#include "service.h"

namespace sml
{
logger log;
ring input_ring, output_ring;

service::service(asio::io_context &io_context, const configuration &conf)
    : io_context_(io_context)
    , conf_(conf)
    , udp_layer_(*this)
    , timer_(io_context_)
    , peer_list_(boost::make_shared<std::vector<address>>())
    , new_peer_list_(boost::make_shared<std::vector<address>>())
{
    udp_layer_.register_filter(bind(&service::server_datagram_filter, this, _1, _2), conf_.server());
    timer_.expires_from_now(boost::posix_time::seconds(0));
    timer_.async_wait(bind(&service::query_server, this, boost::asio::placeholders::error()));
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

asio::io_context &service::io_context()
{
    return io_context_;
}

configuration &service::conf()
{
    return conf_;
}

shared_ptr<std::vector<address>> service::peer_list() const
{
    mutex::scoped_lock _(mutex_);
    return peer_list_;
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

void service::query_server(const boost::system::error_code &ec)
{
    if (!ec)
    {
        mutex::scoped_lock _(mutex_);
        if (new_peer_list_->size())
        {
            peer_list_ = new_peer_list_;
            new_peer_list_ = boost::make_shared<std::vector<address>>();
        }
        timer_.expires_from_now(boost::posix_time::seconds(5));
        timer_.async_wait(bind(&service::query_server, this, boost::asio::placeholders::error()));

        asio::ip::udp::resolver resolver(io_context_);
        asio::ip::udp::resolver::query query(asio::ip::host_name(), "");
        asio::ip::udp::resolver::iterator it = resolver.resolve(query);
        udp_layer_.send_to(boost::make_shared<std::string>(it->endpoint().address().to_string() + ":" + std::to_string(conf_.port())),
                           conf_.server(), [](shared_ptr<std::string>, const address &){});
    }
}

bool service::server_datagram_filter(shared_ptr<std::string> msg, const address &addr)
{
    new_peer_list_->push_back(address(*msg));
    return false;
}
} // namespace sml
