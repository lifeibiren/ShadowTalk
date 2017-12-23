#include "key.h"

key::key()
{

}

key::key(std::string key_val) :
    key_val_(key_val)
{
}
const std::string &key::get_key_val() const
{
    return key_val_;
}
void key::set_key_val(const std::string &key_val)
{
    key_val_ = key_val;
}
