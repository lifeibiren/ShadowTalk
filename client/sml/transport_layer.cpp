#include "transport_layer.h"

namespace sml {
transport_layer::transport_layer(shared_ptr<udp_layer> udp_layer) :
    udp_layer_(udp_layer)
{
}
void transport_layer::send(shared_ptr<message> msg, handler_type handler)
{

}
void transport_layer::recv(shared_ptr<message> msg, handler_type handler)
{

}
}
