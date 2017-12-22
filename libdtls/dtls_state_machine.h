#ifndef DTLS_STATE_MACHINE_H
#define DTLS_STATE_MACHINE_H

#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <string>
#include "record.h"

class dtls_state_machine
{
public:
    dtls_state_machine();
    void feed(boost::shared_ptr<std::string> bytes);

private:
    void finished_handler(boost::shared_ptr<std::string> bytes);
    void preparing_handler(boost::shared_ptr<std::string> bytes);
    void sending_handler(boost::shared_ptr<std::string> bytes);
    void waiting_handler(boost::shared_ptr<std::string> bytes);

    boost::function<void (boost::shared_ptr<std::string> bytes)> handler_;
    boost::function<void (boost::shared_ptr<record> record)> finish_handler_;
};

#endif // DTLS_STATE_MACHINE_H
