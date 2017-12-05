#ifndef BIG_INTEGER_H
#define BIG_INTEGER_H

#include <vector>

class big_integer
{
public:
    big_integer();
    big_integer(const big_integer &val);
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
private:
    std::vector<unsigned long> val_;
};

#endif // BIG_INTEGER_H
