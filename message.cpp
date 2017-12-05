#include "message.h"

message::message()
{

}
message::message(boost::shared_ptr<std::string> content, boost::shared_ptr<peer> &src, boost::shared_ptr<peer> &dst) :
    content_(content), src_(src), dst_(dst)
{

}
boost::shared_ptr<std::string> message::to_raw_bytes() const
{
    return bytes_;
}
