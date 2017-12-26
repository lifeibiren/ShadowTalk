#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <iostream>

namespace shadowtalk {
template <typename T>
class exception {
public:
    exception() {}
    exception(T t) : t_(t)
    {
        std::cerr<<t_<<std::endl;
    }
    virtual ~exception() {}
    friend exception<T> &operator<<(exception<T> &e, const T &t)
    {
        e.t_ = t;
        return e;
    }
private:
    T t_;
};
} //namespace shadowtalk

#endif // EXCEPTION_H
