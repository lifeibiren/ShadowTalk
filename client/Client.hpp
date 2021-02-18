#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/asio/ssl.hpp>

#include <cstdlib>
#include <cstring>
#include <functional>
#include <iostream>
#include <sstream>

class client;

class Peer {
public:
    std::string name;
    std::string address;
};

class NetworkService {
    std::string server_;
    std::uint16_t port_;
    boost::asio::io_context io_context_;
    boost::asio::ssl::context ctx_;
    std::unique_ptr<client> c_;
    std::thread t_;

public:
    NetworkService(
        const std::string &server, std::uint16_t port, const std::string &ca)
        : server_(server)
        , port_(port)
        , ctx_(boost::asio::ssl::context::tlsv13) {
        ctx_.load_verify_file(ca);
#if 0
        ctx.use_certificate_file(
            "GhostApple.crt", boost::asio::ssl::context::file_format::pem);
        ctx.use_private_key_file(
            "GhostApple.key", boost::asio::ssl::context::file_format::pem);
#endif
        t_ = std::thread(std::bind(&NetworkService::routine, this));
    }

protected:
    void routine();

    void ListPeers(std::function<void(const std::vector<Peer *> &)> handler);

    bool Send(Peer *p, std::string data);

    void SetRecvCallback(
        std::function<void(Peer *, const std::string &data)> handler);
};
