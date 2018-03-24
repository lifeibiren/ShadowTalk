#ifndef SML_H
#define SML_H
#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

using namespace boost;
typedef shared_ptr<std::string> sptr_string;
typedef shared_ptr<std::string> sptr_bytes;
typedef uint8_t byte;
#include "exception.h"
#include "encrypt_layer.h"
namespace sml
{
class sml
{
public:
    sml();
};
} // namespace sml

#endif // SML_H
