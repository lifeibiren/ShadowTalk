#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <boost/exception/all.hpp>
#include <exception>
#include <iostream>

#if BOOST_VERSION >= 106500
#include <boost/stacktrace.hpp>
#endif

class exception_base : public virtual std::exception, public virtual boost::exception
{
public:
    exception_base()
    {
#if BOOST_VERSION >= 106500
        std::cerr << boost::stacktrace::stacktrace();
        std::cerr.flush();
#endif
    }
    virtual ~exception_base() {}
};

struct mem_error : exception_base
{};
struct access_violation : mem_error
{};

struct io_error : exception_base
{};
struct file_read_error : io_error
{};

struct data_error : exception_base
{};
struct invalid_data_error : data_error
{};
struct duplicate_data_error : data_error
{};
struct nonexistent_data_error : data_error
{};

struct unknown_error : exception_base
{};

#endif // EXCEPTION_H
