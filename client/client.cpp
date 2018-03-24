#include "udp_service.h"
#include "sml/sml.h"
#include <iostream>

void handle_received_message(boost::shared_ptr<std::string> msg)
{
    std::cout << msg << std::endl;
}
byte key[16] = {0x01,0x12,0x23,0x34,0x45,0x56,0x67,0x78,0x89,0x90,0x01,0x12,0x23,0x34,0x45,0x56};

int main(int argc, char** args)
{
    sml::encrypt_layer encrypt_layer_(sml::encrypt_layer::algorithm::AES_128, sptr_string(new std::string((char *)key, 16)));
    sptr_string text(new std::string("Hello world\n"));
    sptr_string cipher = encrypt_layer_.encrypt(text);
    for (int i = 0; i < cipher->size(); i ++)
        printf("%02x ", byte((*cipher)[i]));
    sptr_string plain = encrypt_layer_.decrypt(cipher);
    std::cout<<*plain<<std::endl;

    text = sptr_string(new std::string("Are you nuts?\n"));
    cipher = encrypt_layer_.encrypt(text);
    for (int i = 0; i < cipher->size(); i ++)
        printf("%02x ", byte((*cipher)[i]));
    plain = encrypt_layer_.decrypt(cipher);
    std::cout<<*plain<<std::endl;
    return 0;
}
