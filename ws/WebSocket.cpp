//
// Copyright (c) 2016-2019 Vinnie Falco (vinnie dot falco at gmail dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/beast
//

//------------------------------------------------------------------------------
//
// Example: WebSocket server, coroutine
//
//------------------------------------------------------------------------------

#include <boost/range.hpp>

#include <boost/asio/spawn.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <cstdlib>

#include <algorithm>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

#include "Message.hpp"

namespace beast = boost::beast; // from <boost/beast.hpp>
namespace http = beast::http; // from <boost/beast/http.hpp>
namespace websocket = beast::websocket; // from <boost/beast/websocket.hpp>
namespace net = boost::asio; // from <boost/asio.hpp>
using tcp = boost::asio::ip::tcp; // from <boost/asio/ip/tcp.hpp>

//------------------------------------------------------------------------------

namespace std {
template <>
struct hash<tcp::endpoint> {
    std::size_t operator()(const tcp::endpoint &val) const {
        return std::hash<unsigned long>()(val.address().to_v4().to_ulong());
    }
};
}

// Report a failure
void fail(beast::error_code ec, const std::string &what) {
    std::cerr << what << ": " << ec.message() << "\n";
}

// Echoes back all received WebSocket messages
class Session : public std::enable_shared_from_this<Session> {
public:
    static std::unordered_map<tcp::endpoint, std::weak_ptr<Session>>
        session_table;
    static std::mutex m;

    websocket::stream<beast::tcp_stream> ws_;
    tcp::endpoint endp_;
    whisper::MessageCtx ctx_;

    Session(const Session &) = delete;
    Session(Session &&) = delete;

    Session(tcp::socket &&sock) noexcept
        : ws_(websocket::stream<beast::tcp_stream>(std::move(sock)))
        , endp_(get_lowest_layer(ws_).socket().remote_endpoint()) {
        std::lock_guard g(m);
        session_table.emplace(endp_, weak_from_this());
    }

    ~Session() {
        std::lock_guard g(m);
        session_table.erase(endp_);
    }

    void Start(net::yield_context yield) { do_session(yield); }

    void do_session(net::yield_context yield) {
        auto self(shared_from_this());

        beast::error_code ec;

        // Set suggested timeout settings for the websocket
        ws_.set_option(websocket::stream_base::timeout::suggested(
            beast::role_type::server));

        // Set a decorator to change the Server of the handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::response_type &res) {
                res.set(http::field::server,
                    std::string(BOOST_BEAST_VERSION_STRING)
                        + " websocket-server-coro");
            }));

        // Accept the websocket handshake
        ws_.async_accept(yield[ec]);
        if (ec)
            return fail(ec, "accept");

        for (;;) {
            // This buffer will hold the incoming message
            beast::flat_buffer buffer;

            // Read a message
            ws_.async_read(buffer, yield[ec]);

            // This indicates that the session was closed
            if (ec == websocket::error::closed)
                break;

            if (ec)
                return fail(ec, "read");

            auto data = buffer.data();
            std::list<whisper::Message> lst = ctx_.Feed(std::vector<char>(
                (char *)data.data(), ((char *)data.data()) + data.size()));

            while (!lst.empty()) {
                whisper::Message m = lst.front();
                lst.pop_front();
                ec = handleMessage(m, yield);
                if (ec) {
                    return fail(ec, "handle message");
                }
            }

            // Echo the message back
            // ws_.text(ws_.got_text());
            // ws_.async_write(buffer.data(), yield[ec]);
            // if (ec)
            // return fail(ec, "write");
        }
    }
    beast::error_code handleMessage(
        const whisper::Message &m, net::yield_context yield) {
        whisper::Message reply;
        switch (m.type) {
            case whisper::MessageType::NOP:
            case whisper::MessageType::PING:
                reply = handlePING(m);
                break;
            case whisper::MessageType::LIST_PEERS:
                reply = handleLISTPEERS(m);
                break;
            case whisper::MessageType::INVALID:
            default:
                break;
        }

        beast::error_code ec;
        std::stringstream buf;
        msgpack::pack(buf, reply);
        std::string str = buf.str();
        ws_.async_write(boost::asio::buffer(str.data(), str.size()), yield[ec]);
        return ec;
    }

    whisper::Message handlePING(const whisper::Message &m) {
        return whisper::Message::CreateMessage(whisper::MessageType::PONG);
    }

    whisper::Message handleLISTPEERS(const whisper::Message &m) {
        return whisper::Message::CreateMessage(whisper::MessageType::PONG);
    }
};

std::unordered_map<tcp::endpoint, std::weak_ptr<Session>>
    Session::session_table;
std::mutex Session::m;

//------------------------------------------------------------------------------

// Accepts incoming connections and launches the sessions
void do_listen(
    net::io_context &ioc, tcp::endpoint endpoint, net::yield_context yield) {
    beast::error_code ec;

    // Open the acceptor
    tcp::acceptor acceptor(ioc);
    acceptor.open(endpoint.protocol(), ec);
    if (ec)
        return fail(ec, "open");

    // Allow address reuse
    acceptor.set_option(net::socket_base::reuse_address(true), ec);
    if (ec)
        return fail(ec, "set_option");

    // Bind to the server address
    acceptor.bind(endpoint, ec);
    if (ec)
        return fail(ec, "bind");

    // Start listening for connections
    acceptor.listen(net::socket_base::max_listen_connections, ec);
    if (ec)
        return fail(ec, "listen");

    for (;;) {
        tcp::socket socket(ioc);
        acceptor.async_accept(socket, yield[ec]);
        if (ec)
            fail(ec, "accept");
        else {
            std::shared_ptr<Session> s
                = std::make_shared<Session>(std::move(socket));
            boost::asio::spawn(acceptor.get_executor(),
                std::bind(&Session::Start, s, std::placeholders::_1));
        }
    }
}

int main(int argc, char *argv[]) {
    // Check command line arguments.
    if (argc != 4) {
        std::cerr << "Usage: websocket-server-coro <address> <port> <threads>\n"
                  << "Example:\n"
                  << "    websocket-server-coro 0.0.0.0 8080 1\n";
        return EXIT_FAILURE;
    }
    auto const address = net::ip::make_address(argv[1]);
    auto const port = static_cast<unsigned short>(std::atoi(argv[2]));
    auto const threads = std::max<int>(1, std::atoi(argv[3]));

    // The io_context is required for all I/O
    net::io_context ioc(threads);

    // Spawn a listening port
    boost::asio::spawn(ioc,
        std::bind(&do_listen, std::ref(ioc), tcp::endpoint{ address, port },
            std::placeholders::_1));

    // Run the I/O service on the requested number of threads
    std::vector<std::thread> v;
    v.reserve(threads - 1);
    for (auto i = threads - 1; i > 0; --i)
        v.emplace_back([&ioc] { ioc.run(); });
    ioc.run();

    return EXIT_SUCCESS;
}