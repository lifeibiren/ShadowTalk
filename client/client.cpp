#if 0
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

    sml::configuration conf(std::string("127.0.0.1:6666"), args[2], priv, pub, port, 30, 5, pk_map);
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
#endif

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>

#include "Message.hpp"

using boost::asio::ip::tcp;
using std::placeholders::_1;
using std::placeholders::_2;
using namespace whisper;

enum { max_length = 1024 };

class client {
public:
    client(boost::asio::io_context &io_context, boost::asio::ssl::context &context,
        const tcp::resolver::results_type &endpoints)
        : socket_(io_context, context) {
        socket_.set_verify_mode(boost::asio::ssl::verify_peer);
        socket_.set_verify_callback(std::bind(&client::verify_certificate, this, _1, _2));

        connect(endpoints);
    }

private:
    bool verify_certificate(bool preverified, boost::asio::ssl::verify_context &ctx) {
        // The verify callback can be used to check whether the certificate that is
        // being presented is valid for the peer. For example, RFC 2818 describes
        // the steps involved in doing this for HTTPS. Consult the OpenSSL
        // documentation for more details. Note that the callback is called once
        // for each certificate in the certificate chain, starting from the root
        // certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        std::cout << "Verifying " << subject_name << "\n";

        return preverified;
    }

    void connect(const tcp::resolver::results_type &endpoints) {
        boost::asio::async_connect(socket_.lowest_layer(), endpoints,
            [this](const boost::system::error_code &error, const tcp::endpoint & /*endpoint*/) {
                if (!error) {
                    handshake();
                } else {
                    std::cout << "Connect failed: " << error.message() << "\n";
                }
            });
    }

    void handshake() {
        socket_.async_handshake(boost::asio::ssl::stream_base::client, [this](const boost::system::error_code &error) {
            if (!error) {
                send_request();
            } else {
                std::cout << "Handshake failed: " << error.message() << "\n";
            }
        });
    }

    void send_request() {
        std::cout << "Enter message: ";
        std::cin.getline(request_, max_length);
        size_t request_length = std::strlen(request_);

        auto m = Message::CreateMessage(Message::MessageType::LIST_PEERS);
        auto buf = Message::ToBytes(m);

        boost::asio::async_write(
            socket_, boost::asio::buffer(buf), [this](const boost::system::error_code &error, std::size_t length) {
                if (!error) {
                    receive_response();
                } else {
                    std::cout << "Write failed: " << error.message() << "\n";
                }
            });
    }

    void receive_response() {
       socket_.async_read_some(boost::asio::buffer(reply_, sizeof(reply_)),
            [this](const boost::system::error_code &error, std::size_t length) {
                if (!error) {
                    auto ret = ctx_.Feed(std::vector<char>(reply_, reply_ + length));
                    std::cout<<"Read data of "<<length <<" bytes"<<"\n";
                    if (ret) {
                        std::cout << "Reply: ";
                        std::cout.write(ret.value().bytes.data(), ret.value().bytes.size());
                        std::cout << "\n";
                    } else {
                        receive_response();
                    }
                } else {
                    std::cout << "Read failed: " << error.message() << "\n";
                }
            });
    }

    boost::asio::ssl::stream<tcp::socket> socket_;
    char request_[max_length];
    char reply_[max_length];

    MessageCtx ctx_;
};

int main(int argc, char *argv[]) {
    try {
        if (argc != 3) {
            std::cerr << "Usage: client <host> <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(argv[1], argv[2]);

        boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv13);
        ctx.load_verify_file("ca.crt");
        ctx.use_certificate_file("GhostApple.crt", boost::asio::ssl::context::file_format::pem);
        ctx.use_private_key_file("GhostApple.key", boost::asio::ssl::context::file_format::pem);

        client c(io_context, ctx, endpoints);

        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}