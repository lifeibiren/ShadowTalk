#include "state_machine.h"

state_machine::state_machine()
{
    handler_ = boost::bind(&state_machine::finished_handler, this, _1);
}
void state_machine::finished_handler(boost::shared_ptr<std::string>)
{

}
void state_machine::preparing_handler(boost::shared_ptr<std::string>)
{

}
void state_machine::sending_handler(boost::shared_ptr<std::string>)
{

}
void state_machine::waiting_handler(boost::shared_ptr<std::string>)
{

}
