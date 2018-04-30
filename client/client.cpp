#include "sml/sml.h"
#include "udp_service.h"
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
    std::cout<<"there comes a new peer\n";
}

void timer_handler(asio::deadline_timer &timer, shared_ptr<sml::stream> &s)
{
        shared_ptr<std::string> get;
        //(*s) << str;
        (*s) >> get;
        if ((*get) != "") {
            std::cout<< *get<<std::endl;
        }
    timer.expires_from_now(boost::posix_time::seconds(1));
    timer.async_wait(bind(timer_handler, ref(timer), ref(s)));
}

int main(int argc, char** args)
{
    uint16_t port = atoi(args[1]);

    shared_ptr<sml::service> service = make_shared<sml::service>(port);
    service->init();
    service->async_accept_peer(peer_handler);
    sml::address addr("127.0.0.1", port ^ 1);
    shared_ptr<sml::peer> q = service->create_peer(addr);
    shared_ptr<sml::stream> s = q->create_stream(0);
    shared_ptr<std::string> str = make_shared<std::string>("Helloworld\n");
    (*s) << str;
    //boost::thread t(boost::bind(&boost::asio::io_context::run, &sml::sml_io_context));
    asio::deadline_timer timer(sml::sml_io_context);
    timer.expires_from_now(boost::posix_time::seconds(1));
    timer.async_wait(bind(timer_handler, ref(timer), ref(s)));
    service->start();

    //    sml::encrypt_layer encrypt_layer_(
    //        sml::encrypt_layer::algorithm::AES_128, sptr_string(new std::string((char*)key, 16)));
    //    sptr_string text(new std::string("Hello world\n"));
    //    sptr_string cipher = encrypt_layer_.encrypt(text);
    //    for (int i = 0; i < cipher->size(); i++)
    //        printf("%02x ", byte((*cipher)[i]));
    //    sptr_string plain = encrypt_layer_.decrypt(cipher);
    //    std::cout << *plain << std::endl;

    //    text = sptr_string(new std::string("Are you nuts?\n"));
    //    cipher = encrypt_layer_.encrypt(text);
    //    for (int i = 0; i < cipher->size(); i++)
    //        printf("%02x ", byte((*cipher)[i]));
    //    plain = encrypt_layer_.decrypt(cipher);
    //    std::cout << *plain << std::endl;
    return 0;
}
