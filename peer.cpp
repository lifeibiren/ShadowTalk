#include "peer.h"

peer::peer()
{

}
std::string peer::address() const
{
    return address_;
}
unsigned int peer::port() const
{
    return port_;
}
