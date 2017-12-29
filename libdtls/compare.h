#ifndef COMPARE_H
#define COMPARE_H

class compare
{
public:
    template <typename T>
    static T sum(const T &t)
    {
        return t;
    }
    template <typename T, typename...Args>
    static T sum(const T &t, const Args&...rest)
    {
        return t + sum(rest...);
    }

    template <typename T>
    static T min(const T &t)
    {
        return t;
    }
    template <typename T, typename...Args>
    static T min(const T &t, const Args&...rest)
    {
        T min_rest = min(rest...);
        return t < min_rest ? t : min_rest;
    }

    template <typename T>
    static T max(const T &t)
    {
        return t;
    }
    template <typename T, typename...Args>
    static T max(const T &t, const Args&...rest)
    {
        T max_rest = max(rest...);
        return t > max_rest ? t : max_rest;
    }
};

#endif // COMPARE_H
