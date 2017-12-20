#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>

namespace shadowtalk {
template <typename T>
class exception {
public:
    exception(T t) : t_(t)
    {
        std::cerr<<t_<<std::endl;
    }
    virtual ~exception() {}
private:
    T t_;
};
} //namespace shadowtalk

#endif // EXCEPTION_H
