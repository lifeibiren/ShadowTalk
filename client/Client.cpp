
#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <qapplication.h>
#include <sstream>

#include "Client.hpp"
#include "Message.hpp"

#include "MainWindow.h"

#if 0

#include <boost/range.hpp>

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ssl.hpp>

using boost::asio::ip::tcp;
using namespace whisper;

enum { max_length = 1024 };

class client {
public:
    client(boost::asio::io_context &io_context,
        boost::asio::ssl::context &context,
        const tcp::resolver::results_type &endpoints)
        : socket_(io_context, context)
        , remote_(endpoints)
        , timer_(io_context) {
        socket_.set_verify_mode(boost::asio::ssl::verify_peer);
        socket_.set_verify_callback(std::bind(&client::verify_certificate, this,
            std::placeholders::_1, std::placeholders::_2));

        boost::asio::spawn(
            std::bind(&client::routine, this, std::placeholders::_1));
        // connect(endpoints);
    }

private:
    void routine(boost::asio::yield_context yield) {
        connect(yield);
        handshake(yield);
        boost::asio::spawn([this](boost::asio::yield_context yield) {
            receive_response(yield);
        });
        for (;;) {
            bool would_sleep = false;
            {
                std::lock_guard g(m_);
                if (to_write_.empty()) {
                    would_sleep = true;
                }
            }
            if (would_sleep) {
                timer_.expires_from_now(boost::posix_time::millisec(10));
            } else {
                Message m;
                {
                    std::lock_guard g(m_);
                    m = to_write_.front();
                    to_write_.pop_front();
                }
            }
        }
    }

    bool verify_certificate(
        bool preverified, boost::asio::ssl::verify_context &ctx) {
        // The verify callback can be used to check whether the certificate that
        // is being presented is valid for the peer. For example, RFC 2818
        // describes the steps involved in doing this for HTTPS. Consult the
        // OpenSSL documentation for more details. Note that the callback is
        // called once for each certificate in the certificate chain, starting
        // from the root certificate authority.

        // In this example we will simply print the certificate's subject name.
        char subject_name[256];
        X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
        X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
        std::cout << "Verifying " << subject_name << "\n";

        return preverified;
    }

    void connect(boost::asio::yield_context yield) {
        boost::asio::async_connect(socket_.lowest_layer(), remote_, yield);
    }

    void handshake(boost::asio::yield_context yield) {
        socket_.async_handshake(boost::asio::ssl::stream_base::client, yield);
    }

    void send_request(boost::asio::yield_context yield) {
        self_ = "GhostApple";
        std::cout << "Enter message: ";
        std::cin.getline(request_, max_length);
        size_t request_length = std::strlen(request_);

        auto m = Message::CreateMessage(MessageType::LIST_PEERS);
        std::stringstream buf;
        msgpack::pack(buf, m);

        m = Message::CreateMessage(
            MessageType::SEND_MSG, std::string(request_, request_length));
        m.dst = self_;
        m.src = self_;
        msgpack::pack(buf, m);

        std::string str = buf.str();
        boost::asio::async_write(socket_, boost::asio::buffer(str), yield);
    }

    void receive_response(boost::asio::yield_context yield) {
        std::size_t length = socket_.async_read_some(
            boost::asio::buffer(reply_, sizeof(reply_)), yield);

        std::list<Message> msg_list
            = ctx_.Feed(std::vector<char>(reply_, reply_ + length));
        std::cout << "Read data of " << length << " bytes"
                  << "\n";
        for (const auto &m : msg_list) {
            std::cout << "Reply: ";
            std::cout.write(m.bytes.data(), m.bytes.size());
            std::cout << "\n";
        }
    }

    boost::asio::ip::tcp::resolver::results_type remote_;
    boost::asio::ssl::stream<tcp::socket> socket_;

    boost::asio::deadline_timer timer_;

    char request_[max_length];
    char reply_[max_length];

    std::string self_;
    MessageCtx ctx_;

    std::mutex m_;
    std::list<Message> recv_list_;
    std::list<Message> to_write_;
};

void NetworkService::routine() {
    tcp::resolver resolver(io_context_);
    auto endpoints = resolver.resolve(server_, std::to_string(port_));
    c_ = std::make_unique<client>(io_context_, ctx_, endpoints);
    for (;;) {
        io_context_.run();
    }
}

void NetworkService::ListPeers(
    std::function<void(const std::vector<Peer *> &)> handler) {}

bool NetworkService::Send(Peer *p, std::string data) {
    return false;
}

#endif
// std::vector<std::string> NetworkService::receive() {

// }

int main(int argc, char *argv[]) {
    // try {
    //     if (argc != 3) {
    //         std::cerr << "Usage: client <host> <port>\n";
    //         return 1;
    //     }

    //     boost::asio::io_context io_context;

    //     tcp::resolver resolver(io_context);
    //     auto endpoints = resolver.resolve(argv[1], argv[2]);

    //     boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv13);
    //     ctx.load_verify_file("ca.crt");
    //     ctx.use_certificate_file(
    //         "GhostApple.crt", boost::asio::ssl::context::file_format::pem);
    //     ctx.use_private_key_file(
    //         "GhostApple.key", boost::asio::ssl::context::file_format::pem);

    //     client c(io_context, ctx, endpoints);

    //     io_context.run();
    // } catch (std::exception &e) {
    //     std::cerr << "Exception: " << e.what() << "\n";
    // }

    QApplication app(argc, argv);
    MainWindow w;
    w.show();
    return app.exec();
}