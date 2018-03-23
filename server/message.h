#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include "boost/shared_ptr.hpp"
#include "peer.h"


namespace whisper {
class message
{
public:
    void set_content(boost::shared_ptr<std::string> content);
    void set_dst_peer(boost::shared_ptr<peer> dst_peer);
    void set_src_peer(boost::shared_ptr<peer> src_peer);
    boost::shared_ptr<peer> src_peer() const;
    boost::shared_ptr<peer> dst_peer() const;

    boost::shared_ptr<std::string> content() const;
private:
    friend class sml;
    message(boost::shared_ptr<peer> src_peer = boost::shared_ptr<peer>(nullptr),
            boost::shared_ptr<std::string> content = boost::shared_ptr<std::string>(nullptr),
            boost::shared_ptr<peer> dst_peer = boost::shared_ptr<peer>(nullptr)
            );

    boost::shared_ptr<std::string> content_;
    boost::shared_ptr<peer> src_peer_, dst_peer_;
};
} //namespace shadowtalk

#endif // MESSAGE_H
