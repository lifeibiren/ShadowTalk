#pragma once

#include <ctime>
#include <list>
#include <map>
#include <memory>
#include <string>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>

#include <boost/endian/arithmetic.hpp>

#include <nlohmann/json.hpp>

#include "Message.hpp"

namespace whisper {
class peer {
public:
    peer();
    peer(const std::string &address, unsigned int port);
    const std::string &address() const;
    unsigned int port() const;
    void update();
    bool dead() const;
    std::string to_string() const;

    bool operator=(const peer &r_peer);
    bool operator==(const peer &r_peer) const;
    bool operator!=(const peer &r_peer) const;
    bool operator<(const peer &r_peer) const;
    bool operator>(const peer &r_peer) const;

private:
    const int timeout = 30;
    std::string name_;
    std::string address_;
    unsigned int port_;

    time_t last_beat_;
};

class session;
struct Message;
using PeerSessions = std::map<std::string, std::weak_ptr<session>>;

class session : public std::enable_shared_from_this<session> {
public:
    session(std::map<std::string, std::weak_ptr<session>> &sessions, boost::asio::ip::tcp::socket socket,
        boost::asio::ssl::context &context);

    void start();

    ~session();

    const std::list<std::string> &GetCommonName() const noexcept;

    void WriteMessage(Message &&m);

private:
    void do_handshake();
    void do_read();
    void do_write();

    std::string addr_;

    std::list<std::string> cn_;
    PeerSessions &sessions_;
    boost::asio::ssl::stream<boost::asio::ip::tcp::socket> socket_;

    std::array<char, 1024 * 1024> buf_;

    // For reading
    MessageCtx ctx_;

    // For writing
    bool writing_started_;
    std::list<Message> to_write_;
};

} // namespace shadowtalk
