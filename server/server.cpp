#include <cstdlib>
#include <ctime>
#include <functional>
#include <iostream>
#include <list>
#include <optional>
#include <string>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/endian/arithmetic.hpp>
#include <boost/shared_ptr.hpp>

#include <nlohmann/json.hpp>

#include "Message.hpp"
#include "peer.h"

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

using namespace whisper;

namespace nlohmann {
template <>
struct adl_serializer<PeerSessions> {
    static void to_json(json &j, const PeerSessions &opt) {
        for (const auto &[addr, s] : opt) {
            auto p = s.lock();
            if (!p) {
                continue;
            }
            j[addr]["CommonName"] = p->GetCommonName();
        }
    }
};
}

using boost::asio::ip::tcp;

session::session(
    std::map<std::string, std::weak_ptr<session>> &sessions, tcp::socket socket, boost::asio::ssl::context &context)
    : sessions_(sessions)
    , socket_(std::move(socket), context) {}

void session::start() {
    do_handshake();
}

session::~session() {
    sessions_.erase(addr_);
}

const std::list<std::string> &session::GetCommonName() const noexcept {
    return cn_;
}
void session::do_handshake() {
    auto self(shared_from_this());
    socket_.async_handshake(
        boost::asio::ssl::stream_base::server, [this, self](const boost::system::error_code &error) {
            if (!error) {
                char subject_name[256];
                SSL *ssl = socket_.native_handle();
                X509 *cert = SSL_get_peer_certificate(ssl);

                // X509 *cert = X509_STORE_CTX_get_current_cert(ssl);
                X509_NAME *nm = X509_get_subject_name(cert);

                X509_NAME_oneline(nm, subject_name, 256);

                std::cout << "Peer: " << subject_name << "\n";
                // X509 *x509 = SSL_get_peer_certificate(ssl);

                std::cout << "==============\n";
                int lastpos = -1;

                for (;;) {
                    lastpos = X509_NAME_get_index_by_NID(nm, NID_commonName, lastpos);
                    if (lastpos == -1)
                        break;
                    X509_NAME_ENTRY *e = X509_NAME_get_entry(nm, lastpos);
                    ASN1_STRING *str = X509_NAME_ENTRY_get_data(e);
                    int len = ASN1_STRING_length(str);
                    const unsigned char *data = ASN1_STRING_get0_data(str);
                    std::string cn((const char *)data, len);
                    std::cout << cn << std::endl;

                    cn_.emplace_back(std::move(cn));
                }
                std::cout << "==============\n";

                addr_ = socket_.next_layer().remote_endpoint().address().to_string() + ":"
                    + std::to_string(socket_.next_layer().remote_endpoint().port());

                std::cout << "Name: " << addr_ << "\n";

                sessions_.emplace(addr_, weak_from_this());
                do_read();
            }
        });
}

void session::do_read() {
    auto self(shared_from_this());
    socket_.async_read_some(
        boost::asio::buffer(buf_), [this, self](const boost::system::error_code &ec, std::size_t length) {
            std::cout << "Read data of " << length << " bytes" << std::endl;
            if (!ec && length > 0) {
                auto msg = ctx_.Feed(std::vector<char>(buf_.data(), buf_.data() + length));
                if (msg) {
                    Message &m = msg.value();
                    std::optional<Message> to_send;
                    std::cout << "Message type: " << (int)m.type << std::endl;
                    switch (m.type) {
                        case Message::MessageType::PING:
                            to_send = Message::CreateMessage(Message::MessageType::PONG);
                            break;
                        case Message::MessageType::SEND_MSG:
                            // Transfer to specific peers
                            break;
                        case Message::MessageType::LIST_PEERS: {
                            // Return peer list
                            nlohmann::json j;
                            j = sessions_;
                            std::string r = j.dump();
                            to_send = Message::CreateMessage(
                                Message::MessageType::PEER_LIST, std::vector<char>(r.data(), r.data() + r.size()));
                            break;
                        }
                        default:
                        case Message::MessageType::NOP:
                        case Message::MessageType::PONG:
                            // Ignore
                            break;
                    }
                    if (to_send) {
                        WriteMessage(std::move(to_send.value()));
                    }
                }
                do_read();
            }
        });
}

void session::WriteMessage(Message &&m) {
    to_write_.emplace_back(std::move(m));
    if (!writing_started_) {
        do_write();
    }
}

void session::do_write() {
    auto self(shared_from_this());
    if (to_write_.empty()) {
        writing_started_ = false;
        return;
    }
    writing_started_ = true;
    Message m = to_write_.front();
    to_write_.pop_front();
    std::vector<char> bytes = Message::ToBytes(m);
    boost::asio::async_write(
        socket_, boost::asio::buffer(bytes), [this, self](const boost::system::error_code &ec, std::size_t /*length*/) {
            if (!ec) {
                do_write();
            }
        });
}

class server {
public:
    server(boost::asio::io_context &io_context, unsigned short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
        , context_(boost::asio::ssl::context::sslv23) {
        context_.set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2
            | boost::asio::ssl::context::single_dh_use);
        context_.load_verify_file("ca.crt");
        context_.use_certificate_file("WhisperServer.crt", boost::asio::ssl::context::file_format::pem);
        context_.use_private_key_file("WhisperServer.key", boost::asio::ssl::context::pem);
        context_.set_verify_mode(boost::asio::ssl::verify_fail_if_no_peer_cert | boost::asio::ssl::verify_peer);
        // context_.use_tmp_dh_file("dh2048.pem");

        do_accept();
    }

private:
    std::string get_password() const { return "test"; }

    void do_accept() {
        acceptor_.async_accept([this](const boost::system::error_code &error, tcp::socket socket) {
            if (!error) {
                std::make_shared<session>(sessions_, std::move(socket), context_)->start();
            }

            do_accept();
        });
    }

    tcp::acceptor acceptor_;
    std::map<std::string, std::weak_ptr<session>> sessions_;
    boost::asio::ssl::context context_;
};

int main(int argc, char *argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        boost::asio::io_context io_context;

        using namespace std; // For atoi.
        server s(io_context, atoi(argv[1]));

        io_context.run();
    } catch (std::exception &e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
