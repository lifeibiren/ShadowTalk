#ifndef SML_H
#define SML_H

#include <boost/shared_ptr.hpp>
using namespace boost;
#include <string>

typedef shared_ptr<std::string> sptr_string;


#include "encryptor.h"
#include "encrypt_layer.h"
#include "aes_256.h"

namespace sml {
class sml
{
public:
    sml();
};
} //namespace sml

#endif // SML_H
