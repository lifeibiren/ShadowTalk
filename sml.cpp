#include "sml.h"

#include <iostream>
#include <printf.h>

namespace shadowtalk {
sml::sml(unsigned short port) :
    server_(io_service_, port),
    thread_ptr_(new boost::thread(boost::bind(&boost::asio::io_service::run,
                                              &io_service_,
                                              thread_err_)))
{
    server_.register_receive_handler(boost::bind(&sml::receive_handler, this, _1, _2, _3, _4));
    server_.register_send_handler(boost::bind(&sml::send_handler, this, _1, _2, _3, _4));
}
void sml::send_message(boost::shared_ptr<message> msg)
{
    to_be_sent.insert(std::pair<boost::shared_ptr<std::string>, boost::shared_ptr<message>>(msg->content(),msg));

    boost::asio::ip::udp::endpoint endpoint(
                boost::asio::ip::address::from_string(
                    msg->dst_peer()->address()), msg->dst_peer()->port());
    server_.send_to(msg->content(), endpoint);

    printf("0x%016llx\n", (long)&(*(msg->content())));
}
void sml::register_receive_handler(const message_signal_handler &slot)
{
    receive_signal.connect(slot);
}
void sml::register_send_handler(const message_signal_handler &slot)
{
    send_signal.connect(slot);
}
boost::shared_ptr<message> sml::prepare_empty_message() const
{
    boost::shared_ptr<message> msg(new message(self));
    return msg;
}
//boost::shared_ptr<message> sml::prepare_for_handler(boost::shared_ptr<std::string> bytes,
//                                                    boost::asio::ip::udp::endpoint &remote_endpoint,
//                                                    const boost::system::error_code& error,
//                                                    std::size_t bytes_transferred)
//{
//    boost::shared_ptr<message> msg = prepare_empty_message();
//    msg->set_content(bytes);

//    peer_endpoint_bm_type::left_map::const_iterator it;
//    if ((it = peer_endpoint_bm_.left.find(remote_endpoint)) == peer_endpoint_bm_.left.end()) {
//        boost::shared_ptr<peer> new_peer(new peer(remote_endpoint.address().to_string(), remote_endpoint.port()));
//        peer_endpoint_bm_.insert(peer_endpoint_bm_type::value_type(remote_endpoint, new_peer));
//        msg->set_dst_peer(new_peer);
//    } else {
//        msg->set_dst_peer(it->second);
//    }
//    return msg;
//}
void sml::receive_handler(boost::shared_ptr<std::string> bytes,
                              boost::asio::ip::udp::endpoint &remote_endpoint,
                              const boost::system::error_code& error,
                              std::size_t bytes_transferred)
{
    boost::shared_ptr<message> msg = prepare_empty_message();
    msg->set_content(bytes);

    peer_endpoint_bm_type::left_map::const_iterator it;
    if ((it = peer_endpoint_bm_.left.find(remote_endpoint)) == peer_endpoint_bm_.left.end()) {
        boost::shared_ptr<peer> new_peer(new peer(remote_endpoint.address().to_string(), remote_endpoint.port()));
        peer_endpoint_bm_.insert(peer_endpoint_bm_type::value_type(remote_endpoint, new_peer));
        msg->set_src_peer(new_peer);
    } else {
        msg->set_src_peer(it->second);
    }
    receive_signal(msg);
}
void sml::send_handler(boost::shared_ptr<std::string> bytes,
                              boost::asio::ip::udp::endpoint &remote_endpoint,
                              const boost::system::error_code& error,
                              std::size_t bytes_transferred)
{
    auto it = to_be_sent.find(bytes);
    if (it != to_be_sent.end()) {
        send_signal(it->second);
        std::cout<<"delete"<<std::endl;
    }
    printf("Complete 0x%016llx\n", (long)&(*bytes));
    std::cout.flush();
}
} //namespace shadowtalk
