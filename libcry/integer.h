#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <string>
#include <vector>
namespace cry
{

class Integer
{
public:
    Integer();
    Integer(const Integer& integer);
    Integer(const std::string& num, int base);
    Integer& operator=(Integer& integer);

    Integer& operator+(Integer& integer);
    Integer& operator-(Integer& integer);
    Integer& operator*(Integer& integer);
    Integer& operator/(Integer& integer);
    Integer& operator%(Integer& integer);
    Integer& operator+=(Integer& integer);
    Integer& operator-=(Integer& integer);
    Integer& operator*=(Integer& integer);
    Integer& operator/=(Integer& integer);
    Integer& operator%=(Integer& integer);

    Integer& operator++(); // ++ big_integer
    Integer& operator--(); // -- big_integer
    const Integer operator++(int); // big_integer ++
    const Integer operator--(int); // big_integer --

    bool operator==(Integer& integer);
    bool operator!=(Integer& integer);
    bool operator<(Integer& integer);
    bool operator>(Integer& integer);
    bool operator<=(Integer& integer);
    bool operator>=(Integer& integer);

    bool is_nagetive() const;
    bool is_positive() const;
    int from_string(const std::string& num, int base);
    std::string to_string(int base) const;

private:
    int sign_;
    std::vector<std::uint32_t> val_;
};

} // namespace cry

#endif // BIG_INTEGER_H
