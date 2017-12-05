#ifndef RSA_H
#define RSA_H

#include <boost/array.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/shared_ptr.hpp>
#include <string>

#include "key.h"
#include "big_integer.h"
class rsa
{
public:
    static void keygen(key &pubkey, key &privkey);
    static void encrypt(boost::shared_ptr<std::string> buf, key &pubkey);
    static void decrypt(boost::shared_ptr<std::string> buf, key &privkey);
private:
};

#endif // RSA_H
