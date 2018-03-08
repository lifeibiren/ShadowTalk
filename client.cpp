
#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

#include <ctime>
#include <iostream>
#include <string>
#include <list>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include "udp_server.h"
#include "sml.h"
#include "message.h"
#include "peer.h"
void handle_received_message(boost::shared_ptr<whisper::message> msg)
{
    std::cout<<*(msg->content())<<std::endl;
}
int main(int argc, char **args)
{
    whisper::sml sml_(6665);
    sml_.register_receive_handler(handle_received_message);
    boost::chrono::seconds period(1);
    while(1) {
        boost::shared_ptr<whisper::message> new_msg = sml_.prepare_empty_message();
        boost::shared_ptr<std::string> content(new std::string("Hello world"));
        new_msg->set_content(content);
        boost::shared_ptr<whisper::peer> dst_peer(new whisper::peer("127.0.0.1", 6666));
        new_msg->set_dst_peer(dst_peer);
        sml_.send_message(new_msg);
        boost::this_thread::sleep_for(period);
    }
    return 0;
}
