#ifndef FINISHED_H
#define FINISHED_H

#include "dtls.h"



class finished : public body
{
public:
    finished();
    std::vector<opaque> verify_data_;

    static const std::string finished_label_;
};
const std::string finished::finished_label_ = "client finished";

#endif // FINISHED_H
