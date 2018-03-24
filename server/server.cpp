#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <ctime>
#include <iostream>
#include <list>
#include <string>

#include "message.h"
#include "sml.h"
#include "udp_service.h"

using boost::asio::ip::udp;

whisper::sml sml_(6666);
void handle_client_request(boost::shared_ptr<whisper::message> msg)
{
    for (auto it = sml_.live_peers().begin(); it != sml_.live_peers().end(); ++it)
    {
        boost::shared_ptr<std::string> new_message(new std::string((*it)->to_string()));
        boost::shared_ptr<whisper::message> new_msg = sml_.prepare_empty_message();
        new_msg->set_content(new_message);
        new_msg->set_dst_peer(msg->src_peer());
        sml_.send_message(new_msg);
    }

    std::cout << std::to_string(msg->content()->size()) + " bytes received" << std::endl;
    std::cout << "Requests detected" << std::endl;
}
int main(int argc, char** args)
{
    sml_.register_receive_handler(handle_client_request);
    boost::chrono::seconds period(10);
    while (1)
    {
        boost::this_thread::sleep_for(period);
    }

    return 0;
}
