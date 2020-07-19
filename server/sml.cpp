#include "sml.h"

#include <iostream>
#include <printf.h>

namespace whisper
{
UdpService::UdpService(unsigned short port)
    : server_(io_service_, port)
    , thread_ptr_(new boost::thread(boost::bind(&boost::asio::io_service::run, &io_service_, thread_err_)))
{
    server_.register_receive_handler(boost::bind(&UdpService::receive_handler, this, _1, _2, _3));
    server_.register_send_handler(boost::bind(&UdpService::send_handler, this, _1, _2, _3));
}
void UdpService::send_message(boost::shared_ptr<message> msg)
{
    to_be_sent_.insert(to_be_sent_map_type::value_type(msg->content(), msg));

    peer_endpoint_bm_type::right_map::const_iterator it;
    if ((it = peer_endpoint_bm_.right.find(msg->dst_peer())) == peer_endpoint_bm_.right.end())
    {
        boost::asio::ip::udp::endpoint remote_endpoint(
            boost::asio::ip::address::from_string(msg->dst_peer()->address()), msg->dst_peer()->port());
        peer_endpoint_bm_.insert(peer_endpoint_bm_type::value_type(remote_endpoint, msg->dst_peer()));
        live_peers_.insert(msg->dst_peer());
        server_.send_to(msg->content(), remote_endpoint);
    }
    else
    {
        server_.send_to(msg->content(), it->second);
    }
}
void UdpService::register_receive_handler(const message_signal_handler& slot)
{
    receive_signal_.connect(slot);
}
void UdpService::register_send_handler(const message_signal_handler& slot)
{
    send_signal_.connect(slot);
}
boost::shared_ptr<message> UdpService::prepare_empty_message() const
{
    return boost::shared_ptr<message>(new message(self_));
}
void UdpService::receive_handler(boost::shared_ptr<std::string> bytes, boost::asio::ip::udp::endpoint& remote_endpoint,
    const boost::system::error_code& error)
{
    boost::shared_ptr<message> msg = prepare_empty_message();
    msg->set_content(bytes);

    peer_endpoint_bm_type::left_map::const_iterator it;
    if ((it = peer_endpoint_bm_.left.find(remote_endpoint)) == peer_endpoint_bm_.left.end())
    {
        boost::shared_ptr<peer> new_peer(new peer(remote_endpoint.address().to_string(), remote_endpoint.port()));
        peer_endpoint_bm_.insert(peer_endpoint_bm_type::value_type(remote_endpoint, new_peer));
        live_peers_.insert(new_peer);
        msg->set_src_peer(new_peer);
    }
    else
    {
        msg->set_src_peer(it->second);
        it->second->update();
    }
    receive_signal_(msg);
}
void UdpService::send_handler(boost::shared_ptr<std::string> bytes, boost::asio::ip::udp::endpoint& remote_endpoint,
    const boost::system::error_code& error)
{
    to_be_sent_map_type::iterator it = to_be_sent_.find(bytes);
    if (it != to_be_sent_.end())
    {
        send_signal_(it->second);
        to_be_sent_.erase(it);
    }
}
const UdpService::live_peers_set_type& UdpService::live_peers()
{
    for (std::set<boost::shared_ptr<peer>>::iterator it = live_peers_.begin(); it != live_peers_.end();)
    {
        if ((*it)->dead())
        {
            peer_endpoint_bm_.right.erase(*it);
            it = live_peers_.erase(it);
        }
        else
        {
            ++it;
        }
    }
    std::cout<<live_peers_.size()<<std::endl;
    std::cout.flush();
    return live_peers_;
}
} // namespace shadowtalk
