#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>

namespace sml
{
class logger
{
public:
    logger();
    template <typename T> logger& operator<<(const T& t)
    {
        std::cout << t;
        return *this;
    }
};
}

#endif // LOGGER_H
