#include "sml/service.h"
#include <iostream>

void handle_received_message(boost::shared_ptr<std::string> msg)
{
    std::cout << msg << std::endl;
}
byte key[16] = { 0x01, 0x12, 0x23, 0x34, 0x45, 0x56, 0x67, 0x78, 0x89, 0x90, 0x01, 0x12, 0x23, 0x34, 0x45, 0x56 };

shared_ptr<sml::peer> p;
void peer_handler(shared_ptr<sml::peer> new_peer)
{
    p = new_peer;
    std::cout << "there comes a new peer\n";
}

void timer_handler(asio::deadline_timer &timer, shared_ptr<sml::stream> &s)
{
    shared_ptr<std::string> get;
    //(*s) << str;
    //        (*s) >> get;
    //        if ((*get) != "") {
    //            std::cout<< *get<<std::endl;
    //        }
    timer.expires_from_now(boost::posix_time::seconds(1));
    timer.async_wait(bind(timer_handler, ref(timer), ref(s)));
}

const char *priv = "ed4b008b62dd2563b6406240ba55ee230bda4c1h";
const char *pub = "6e794b0ea27adebc21bcbe0c2ecc1d3cd92baabc91f3"
                  "f375020b8ad07220ca20beffbee1d359de88d820ff1b"
                  "daadb868f2fdec148a9bcfd34b49f4a264a3ab8c9051"
                  "44eda1ef5cf645ddade075e63748597128f83f5ec290"
                  "c8b7808a96b4315ddd6cf8cc8f97b938dbfa6b31d3d8"
                  "2df84c31f6ac55e5362ee417538eb34b419ch";

asio::io_context io_context;
int main(int argc, char **args)
{
    uint16_t port = atoi(args[1]);
    sml::configuration::trusted_peer_key_map_type pk_map;
    pk_map.insert(sml::configuration::trusted_peer_key_map_type::value_type("Bob", pub));
    pk_map.insert(sml::configuration::trusted_peer_key_map_type::value_type("John", pub));

    sml::configuration conf(std::string("45.76.158.199:6666"), args[2], priv, pub, port, 30, 5, pk_map);
    sml::service service(io_context, conf);
    thread t(bind(&asio::io_context::run, &io_context));

    sml::address addr("127.0.0.1", port ^ 1);
    if (port == 8000)
    {
        service.post(make_shared<sml::add_peer>(addr));
        service.post(make_shared<sml::add_stream>(addr, 0));
        service.post(make_shared<sml::send_data>(addr, 0, "Hello World\n"));
    }

    while (1)
    {
        shared_ptr<sml::message> ptr = service.query();
        if (ptr)
        {
            std::cout << typeid(*ptr).name() << std::endl;
            sml::recv_data *data = dynamic_cast<sml::recv_data *>(ptr.get());
            if (data)
            {
                std::cout << "received data:" << std::string(*data) << std::endl;
            }
        }
    }
    return 0;
}
