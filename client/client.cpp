#include "udp_service.h"

#include <iostream>

void handle_received_message(boost::shared_ptr<std::string> msg)
{
    std::cout<<msg<<std::endl;
}
int main(int argc, char **args)
{

    return 0;
}
