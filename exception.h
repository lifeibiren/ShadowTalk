#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>
#include <exception>
#include <boost/exception/all.hpp>

#include "dtls.h"
#if BOOST_VERSION >= 106500
#include <boost/stacktrace.hpp>
#endif

class exception_base :
        virtual std::exception, virtual boost::exception
{
public:
    exception_base() {
        #if BOOST_VERSION >= 106500
        std::cerr << boost::stacktrace::stacktrace();
        std::cerr.flush();
        #endif
    }
};

struct mem_error : virtual exception_base { };
struct access_violation : virtual mem_error { };

struct io_error : virtual exception_base { };
struct file_read_error : virtual io_error { };

struct unknown_error : virtual exception_base { };


#endif // EXCEPTION_H
