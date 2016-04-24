#pragma once 

#include <cmath>

namespace cs224 {

// Generate a random float number between a and b.
// This function generate random float number between
// (0,1) by default.
static inline float randomFloat(float a = 0.f, float b = 1.f) {

    float ran = (float) rand() / (float) RAND_MAX;
    float diff = std::abs(b - a);
    return ran * diff + std::min(a,b);
}

template<typename T> static constexpr T pow2(T x) {
    return x * x;
}

template<typename T> static constexpr T pow3(T x) {
    return x * x * x;
}

template<typename T> static constexpr T pow6(T x) {
    return x * x * x * x * x * x;
}

template<typename T> static constexpr T pow9(T x) {
    return x * x * x * x * x * x * x * x * x;
}

template<typename T> static inline T clamp(T x, T lo, T hi) {
    return std::max(lo, std::min(hi, x));
}

template<typename S, typename T> static inline T lerp(S t, const T &a, const T &b) {
    return (S(1) - t) * a + t * b;
}

static inline float unitToRange(float x, float lo, float hi) {
    return lo + clamp(x, 0.f, 1.f) * (hi - lo);
}

static inline float rangeToUnit(float x, float lo, float hi) {
    return clamp((x - lo) / (hi - lo), 0.f, 1.f);
}

static inline uint32_t nextPowerOfTwo(uint32_t x) {
    x --;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x ++;
    return x;
}

} // namespace cs224
