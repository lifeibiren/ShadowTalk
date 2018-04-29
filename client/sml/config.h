#ifndef CONFIG_H
#define CONFIG_H

#include <list>
#include <map>
#include <string>

#include <boost/any.hpp>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/endian/endian.hpp>
#include <boost/function.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/unordered_map.hpp>
#include <boost/thread.hpp>

#include <cinttypes>
#include <string>
using namespace boost;
using boost::make_shared;
typedef shared_ptr<std::string> sptr_string;
typedef uint8_t byte;

#include "logger.h"
namespace sml
{
extern asio::io_context sml_io_context;
extern logger log;
class peer;
class udp_layer;
class stream;
class service;
class encrypt_layer;
class address;
class logger;
}


template<typename ... Args>
std::string string_format(const std::string &format, Args ... args)
{
    size_t size = snprintf(nullptr, 0, format.c_str(), args ...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, format.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1);
}

#include "exception.h"
#include "utils.h"

#endif // CONFIG_H
