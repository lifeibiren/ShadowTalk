#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <ctime>
#include <iostream>
#include <list>
#include <string>

#if 0
#include "message.h"
#include "sml.h"
#include "udp_service.h"

using boost::asio::ip::udp;

whisper::UdpService sml_(6666);
void handle_client_request(boost::shared_ptr<whisper::message> msg)
{
    auto live_peers = sml_.live_peers();
    for (auto it = live_peers.begin(); it != live_peers.end(); ++it)
    {
        boost::shared_ptr<std::string> new_message(new std::string((*it)->to_string()));
        boost::shared_ptr<whisper::message> new_msg = sml_.prepare_empty_message();
        new_msg->set_content(new_message);
        new_msg->set_dst_peer(msg->src_peer());
        sml_.send_message(new_msg);
    }
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
#endif

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <cstdlib>
#include <functional>
#include <iostream>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session>
{
public:
    session(tcp::socket socket, boost::asio::ssl::context &context)
        : socket_(std::move(socket), context)
    {}

    void start() { do_handshake(); }

private:
    void do_handshake()
    {
        auto self(shared_from_this());
        socket_.async_handshake(
            boost::asio::ssl::stream_base::server, [this, self](const boost::system::error_code &error) {
                if (!error)
                {
                    do_read();
                }
            });
    }

    void do_read()
    {
        auto self(shared_from_this());
        socket_.async_read_some(
            boost::asio::buffer(data_), [this, self](const boost::system::error_code &ec, std::size_t length) {
                if (!ec)
                {
                    do_write(length);
                }
            });
    }

    void do_write(std::size_t length)
    {
        auto self(shared_from_this());
        boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
            [this, self](const boost::system::error_code &ec, std::size_t /*length*/) {
                if (!ec)
                {
                    do_read();
                }
            });
    }

    boost::asio::ssl::stream<tcp::socket> socket_;
    char data_[1024];
};

class server
{
public:
    server(boost::asio::io_context &io_context, unsigned short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , context_(boost::asio::ssl::context::sslv23)
    {
        context_.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);
        context_.use_certificate_file("WhisperServer.crt", boost::asio::ssl::context::file_format::pem);
        context_.use_private_key_file("WhisperServer.key", boost::asio::ssl::context::pem);
        context_.set_verify_mode(/*boost::asio::ssl::verify_fail_if_no_peer_cert | */ boost::asio::ssl::verify_peer);
        // context_.use_tmp_dh_file("dh2048.pem");

        do_accept();
    }

private:
    std::string get_password() const { return "test"; }

    void do_accept()
    {
        acceptor_.async_accept([this](const boost::system::error_code &error, tcp::socket socket) {
            if (!error)
            {
                std::make_shared<session>(std::move(socket), context_)->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    boost::asio::ssl::context context_;
};

int main(int argc, char *argv[])
{
    try
    {
        if (argc != 2)
        {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        using namespace std; // For atoi.
        server s(io_context, atoi(argv[1]));

        io_context.run();
    }
    catch (std::exception &e)
    {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}