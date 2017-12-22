#include "dtls_state_machine.h"

dtls_state_machine::dtls_state_machine()
{
    handler_ = boost::bind(&dtls_state_machine::finished_handler, this, _1);
}
void dtls_state_machine::finished_handler(boost::shared_ptr<std::string>)
{

}
void dtls_state_machine::preparing_handler(boost::shared_ptr<std::string>)
{

}
void dtls_state_machine::sending_handler(boost::shared_ptr<std::string>)
{

}
void dtls_state_machine::waiting_handler(boost::shared_ptr<std::string>)
{

}
