#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <map>
#include <list>

#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/unordered_map.hpp>
#include <boost/endian/endian.hpp>

#include <cinttypes>
#include <string>
using namespace boost;
typedef shared_ptr<std::string> sptr_string;
typedef uint8_t byte;

#include "exception.h"

#endif // CONFIG_H
