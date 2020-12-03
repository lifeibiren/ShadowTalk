#include "ring.h"

namespace sml {
ring::ring() {}

void ring::put(shared_ptr<message> msg) {
    mutex::scoped_lock _(mutex_);
    msg_lst_.push_back(msg);
}

shared_ptr<message> ring::get() {
    mutex::scoped_lock _(mutex_);
    if (msg_lst_.begin() != msg_lst_.end()) {
        shared_ptr<message> ret = *(msg_lst_.begin());
        msg_lst_.pop_front();
        return ret;
    }
    return nullptr;
}

size_t ring::size() const {
    mutex::scoped_lock _(mutex_);
    return msg_lst_.size();
}
}
