export module utils;

export template<typename T>
constexpr T square(T x) {
    return x * x;
}

export template<typename T>
constexpr T cube(T x) {
    return x * x * x;
}

export template<typename T>
constexpr T power(T base, const int exp) {
    T result = 1;
    for (int i = 0; i < exp; ++i) {
        result *= base;
    }
    return result;
}
