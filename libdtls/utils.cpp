#include "utils.h"

std::string utils::convert_endian(std::string &src)
{
    std::string dst(src);
    char tmp;
    for (int i = 0, j = dst.size() - 1; i < j; i ++, j --) {
        tmp = dst[i];
        dst[i] = dst[j];
        dst[j] = tmp;
    }
    return dst;
}
