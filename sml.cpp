#include "sml.h"

#include <iostream>

namespace shadowtalk {
sml::sml(unsigned short port) :
    server_(io_service_, port),
    thread_ptr_(new boost::thread(boost::bind(&boost::asio::io_service::run,
                                              &io_service_,
                                              thread_err_)))
{
    server_.register_receive_handler(boost::bind(&sml::handle_client_request, this, _1, _2, _3, _4));

}
void sml::send_message(boost::shared_ptr<std::string> message, boost::shared_ptr<peer> to)
{
    boost::asio::ip::udp::endpoint endpoint(boost::asio::ip::address::from_string(to->address()), to->port());
    server_.send_to(message, endpoint);
}

void sml::handle_client_request(boost::shared_ptr<std::string> message,
                              boost::asio::ip::udp::endpoint &remote_endpoint,
                              const boost::system::error_code& error,
                              std::size_t bytes_transferred)
{


//    peer_list_.push_back(remote_endpoint.address().to_string() + " " +
//                         std::to_string(remote_endpoint.port()) + " " +
//                         make_daytime_string());
    peer_list_.push_back(peer(remote_endpoint.address().to_string(),
                              remote_endpoint.port()));


    for (std::list<peer>::iterator it = peer_list_.begin();
         it != peer_list_.end();) {
        if (it->dead()) {
            it = peer_list_.erase(it);
        } else {
            boost::shared_ptr<std::string> new_message(new std::string(it->to_string()));
            server_.send_to(new_message, remote_endpoint);
            ++it;
        }
    }
//            boost::shared_ptr<std::string> message(
//                        new std::string(remote_endpoint_.address().to_string() + " " +
//                                        std::to_string(remote_endpoint_.port()) + " " +
//                                        make_daytime_string()));
//    server_.send_to(new_message, remote_endpoint);



    std::cout<<std::to_string(bytes_transferred) + " bytes received"<<std::endl;
}
}
