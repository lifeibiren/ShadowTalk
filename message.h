#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include "boost/shared_ptr.hpp"
#include "peer.h"
#include "key.h"

class message
{
public:
    message();
    message(boost::shared_ptr<std::string> content, boost::shared_ptr<peer> &src, boost::shared_ptr<peer> &dst);

    boost::shared_ptr<std::string> to_raw_bytes() const;
private:
    boost::shared_ptr<std::string> bytes_, content_;
    boost::shared_ptr<key> encrypt_, decrypt_;
    boost::shared_ptr<peer> src_, dst_;

};

#endif // MESSAGE_H
