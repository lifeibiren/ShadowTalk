#ifndef SML_H
#define SML_H

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include "udp_server.h"
#include "message.h"

/**
 * Secure Message Layer
 */

namespace shadowtalk {
class sml
{
public:
    typedef boost::signals2::signal<void (boost::shared_ptr<message> msg)> message_signal;
    typedef message_signal::slot_type message_signal_handler;

    sml(unsigned short port);
    void send_message(boost::shared_ptr<message> msg);
    void register_receive_handler(const message_signal_handler &slot);
    void register_send_handler(const message_signal_handler &slot);

    boost::shared_ptr<message> prepare_empty_message() const;
private:
    message_signal receive_signal;
    message_signal send_signal;

    boost::shared_ptr<message> prepare_for_handler(boost::shared_ptr<std::string> bytes,
                                                   boost::asio::ip::udp::endpoint &remote_endpoint,
                                                   const boost::system::error_code& error,
                                                   std::size_t bytes_transferred);

    void send_handler(boost::shared_ptr<std::string> bytes,
                      boost::asio::ip::udp::endpoint &remote_endpoint,
                      const boost::system::error_code& error,
                      std::size_t bytes_transferred);
    void receive_handler(boost::shared_ptr<std::string> bytes,
                         boost::asio::ip::udp::endpoint &remote_endpoint,
                         const boost::system::error_code& error,
                         std::size_t bytes_transferred);

    boost::asio::io_service io_service_;
    udp_server server_;
    boost::system::error_code thread_err_;
    boost::shared_ptr<boost::thread> thread_ptr_;

    boost::shared_ptr<peer> self;

    template<class Rel>
    struct peer_comp
    {
        bool operator()(const Rel &ra, const Rel &rb) const
        {
            return (*ra) < (*rb);
        }
    };
    typedef boost::bimap
    <
        boost::bimaps::set_of<boost::asio::ip::udp::endpoint>,
        boost::bimaps::set_of<boost::shared_ptr<peer>, peer_comp<boost::shared_ptr<peer>>>
    > peer_endpoint_bm_type;
    peer_endpoint_bm_type peer_endpoint_bm_;

    std::map<boost::shared_ptr<std::string>, boost::shared_ptr<message>> to_be_sent;

    std::list<boost::shared_ptr<peer>> alive_peers_;
    std::list<boost::shared_ptr<peer>> keep_alive_;
    std::list<key> pub_keys_;
};
} //namespace shadowtalk

#endif // SML_H
