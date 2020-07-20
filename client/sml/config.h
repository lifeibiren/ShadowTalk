#ifndef CONFIG_H
#define CONFIG_H

#include <cinttypes>
#include <list>
#include <map>
#include <string>

#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/endian/arithmetic.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/make_unique.hpp>
#include <boost/shared_array.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/unordered_map.hpp>

using namespace boost;
using boost::make_shared;
typedef shared_ptr<std::string> sptr_string;
typedef uint8_t byte;

#include "logger.h"
#include "ring.h"

namespace sml
{
extern logger log;
class ring;
class peer;
class udp_layer;
class stream;
class service;
class encrypt_layer;
class address;
class logger;
extern ring input_ring, output_ring;
}

template <typename... Args>
std::string string_format(const std::string &format, Args... args)
{
    size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

#include "address.h"
#include "exception.h"
#include "utils.h"

#endif // CONFIG_H
