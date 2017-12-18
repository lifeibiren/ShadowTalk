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

int main(int argc, char **args)
{
//    try
//    {
//        boost::asio::io_service io_service;
//        udp_server server(io_service);
//        io_service.run();
//    }
//    catch (std::exception& e)
//    {
//        std::cerr << e.what() << std::endl;
//    }
    shadowtalk::sml sml_(6666);
    boost::chrono::seconds period(10);
    while(1) {
        boost::this_thread::sleep_for(period);
    }
    return 0;
}
