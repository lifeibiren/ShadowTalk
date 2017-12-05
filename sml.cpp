#include "sml.h"

#include <iostream>

sml::sml(unsigned short port) :
    server_(io_service_, port),
    thread_ptr_(new boost::thread(boost::bind(&boost::asio::io_service::run,
                                              &io_service_,
                                              thread_err_)))
{
    server_.register_receive_handler(boost::bind(&sml::handle_time_request, this, _1, _2, _3, _4));

}
int sml::send_message(message &msg, peer &to)
{
    return 0;
}

void sml::handle_time_request(boost::shared_ptr<std::string> message,
                              boost::asio::ip::udp::endpoint &remote_endpoint,
                              const boost::system::error_code& error,
                              std::size_t bytes_transferred)
{
    boost::shared_ptr<std::string> new_message(new std::string());

    host_list_.push_back(remote_endpoint.address().to_string() + " " +
                         std::to_string(remote_endpoint.port()) + " " +
                         make_daytime_string());

    for (std::list<std::string>::iterator it = host_list_.begin();
         it != host_list_.end(); ++it) {
        new_message->append(*it);
    }
//            boost::shared_ptr<std::string> message(
//                        new std::string(remote_endpoint_.address().to_string() + " " +
//                                        std::to_string(remote_endpoint_.port()) + " " +
//                                        make_daytime_string()));
    server_.send_to(new_message, remote_endpoint);



    std::cout<<std::to_string(bytes_transferred) + " bytes received"<<std::endl;
}
