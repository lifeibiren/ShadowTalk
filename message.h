#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>
#include <memory>
#include "peer.h"


class message
{
public:
    message();
    message(std::shared_ptr<std::string> content_, peer &src, peer &dst);
private:
    std::shared_ptr<std::string> content_;
    peer src_, dst_;
};

#endif // MESSAGE_H
