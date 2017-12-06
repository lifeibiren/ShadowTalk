#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>
#include <string>

class big_integer
{
public:
    big_integer();
    big_integer(const big_integer &val);
    big_integer(const std::string &num, int base);
    big_integer &operator= (big_integer &r_val);

    big_integer &operator+ (big_integer &r_val);
    big_integer &operator- (big_integer &r_val);
    big_integer &operator* (big_integer &r_val);
    big_integer &operator/ (big_integer &r_val);
    big_integer &operator% (big_integer &r_val);
    big_integer &operator+= (big_integer &r_val);
    big_integer &operator-= (big_integer &r_val);
    big_integer &operator*= (big_integer &r_val);
    big_integer &operator/= (big_integer &r_val);
    big_integer &operator%= (big_integer &r_val);

    big_integer &operator++ (); // ++ big_integer
    big_integer &operator-- (); // -- big_integer
    const big_integer operator++ (int); // big_integer ++
    const big_integer operator-- (int); // big_integer --

    bool operator== (big_integer &r_val);
    bool operator!= (big_integer &r_val);
    bool operator< (big_integer &r_val);
    bool operator> (big_integer &r_val);
    bool operator<= (big_integer &r_val);
    bool operator>= (big_integer &r_val);

    bool is_nagetive() const;
    bool is_positive() const;
    int from_string(const std::string &num, int base);
    std::string to_string(int base) const;
private:
    int sign_;
    std::vector<std::uint32_t> val_;
};

#endif // BIG_INTEGER_H
