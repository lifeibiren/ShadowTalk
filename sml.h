#ifndef SML_H
#define SML_H

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "udp_server.h"
#include "message.h"
#include "key.h"

namespace shadowtalk {
class sml
{
public:
    sml(unsigned short port);
    void send_message(boost::shared_ptr<std::string> message, boost::shared_ptr<peer> to);
private:
    void handle_client_request(boost::shared_ptr<std::string> message,
                             boost::asio::ip::udp::endpoint &remote_endpoint,
                             const boost::system::error_code& error,
                             std::size_t bytes_transferred);

    boost::asio::io_service io_service_;
    udp_server server_;
    boost::system::error_code thread_err_;
    boost::shared_ptr<boost::thread> thread_ptr_;
    std::list<std::shared_ptr<peer>> alive_peers_;
    std::list<std::shared_ptr<peer>> keep_alive_;
    std::list<key> pub_keys_;
    std::list<peer> peer_list_;
};
}

#endif // SML_H
