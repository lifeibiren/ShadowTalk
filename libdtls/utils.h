#ifndef ENDIAN_H
#define ENDIAN_H

class utils
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

    template <unsigned long B, unsigned long E>
    struct pow
    {
        static const unsigned long result = B * pow<B, E-1>::result;
        constexpr operator unsigned long ()
        {
            return result;
        }
    };

    template <unsigned long B>
    struct pow<B, 0>
    {
        static const unsigned long result = 1;
    };
};

#endif // ENDIAN_H
