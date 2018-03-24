#include "message.h"

namespace whisper
{
message::message(
    boost::shared_ptr<peer> src_peer, boost::shared_ptr<std::string> content, boost::shared_ptr<peer> dst_peer)
    : src_peer_(src_peer)
    , content_(content)
    , dst_peer_(dst_peer)
{}
void message::set_content(boost::shared_ptr<std::string> content)
{
    content_ = content;
}
void message::set_dst_peer(boost::shared_ptr<peer> dst_peer)
{
    dst_peer_ = dst_peer;
}
void message::set_src_peer(boost::shared_ptr<peer> src_peer)
{
    src_peer_ = src_peer;
}
boost::shared_ptr<peer> message::src_peer() const
{
    return src_peer_;
}
boost::shared_ptr<peer> message::dst_peer() const
{
    return dst_peer_;
}
boost::shared_ptr<std::string> message::content() const
{
    return content_;
}
} // namespace shadowtalk
