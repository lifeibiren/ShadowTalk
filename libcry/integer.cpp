#include "integer.h"
namespace cry {


Integer::Integer()
{

}
Integer::Integer(const Integer &integer)
{
    val_ = integer.val_;
}
Integer::Integer(const std::string &num, int base)
{

}
Integer &Integer::operator= (Integer &integer)
{
    val_ = integer.val_;
    return *this;
}
Integer &Integer::operator+ (Integer &integer)
{

}
Integer &Integer::operator- (Integer &integer)
{

}
Integer &Integer::operator* (Integer &integer)
{

}
Integer &Integer::operator/ (Integer &integer)
{

}
Integer &Integer::operator% (Integer &integer)
{

}
Integer &Integer::operator+= (Integer &integer)
{

}
Integer &Integer::operator-= (Integer &integer)
{

}
Integer &Integer::operator*= (Integer &integer)
{

}
Integer &Integer::operator/= (Integer &integer)
{

}
Integer &Integer::operator%= (Integer &integer)
{

}

} //namespace cry
