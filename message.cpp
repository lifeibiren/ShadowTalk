#include "message.h"

message::message()
{

}
message::message(std::shared_ptr<std::string> content, peer &src, peer &dst) :
    content_(content), src_(src), dst_(dst)
{}
