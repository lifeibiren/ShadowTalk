#ifndef RING_H
#define RING_H

#include "config.h"

namespace sml {
class message;
/**
 * @brief
 * thread safe ring buffer
 */
class ring
{

public:
    ring();
    void put(shared_ptr<message> msg);
    shared_ptr<message> get();
    size_t size() const;
private:
    mutable mutex mutex_;
    std::list<shared_ptr<message>> msg_lst_;
};
}

#endif // RING_H
