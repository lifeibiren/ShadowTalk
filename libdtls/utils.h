#ifndef ENDIAN_H
#define ENDIAN_H

#include <string>
class utils
{
public:
    static std::string convert_endian(std::string &src); //big to little or little to big
};

#endif // ENDIAN_H
