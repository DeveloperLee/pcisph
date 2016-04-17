#pragma once 

#include <cmath>

namespace cs224 {

template<typename T>
static constexpr T pow2(T x) { return x*x; }

template<typename T>
static constexpr T pow3(T x) { return x*x*x; }

template<typename T>
static inline T clamp(T x, T lo, T hi)  {
    return std::max(lo, std::min(hi, x));
}

template<typename S, typename T>
static inline T lerp(S t, const T &a, const T &b) {
    return (S(1) - t) * a + t * b;
}

static inline float unitToRange(float x, float lo, float hi) {
    return lo + clamp(x, 0.f, 1.f) * (hi - lo);
}

static inline float rangeToUnit(float x, float lo, float hi) {
    return clamp((x - lo) / (hi - lo), 0.f, 1.f);
}

static inline uint32_t nextPowerOfTwo(uint32_t x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}
class Math {
};
}