#ifndef DLTS_PLAINTEXT_H
#define DLTS_PLAINTEXT_H

#include "record.h"
#include <boost/shared_ptr.hpp>

class plaintext : public record
{
public:
    plaintext();

private:
    boost::shared_ptr<byte_string> fragment_;
};

#endif // DLTS_PLAINTEXT_H
