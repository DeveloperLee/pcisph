// Includes some basic math utility functions.

#pragma once

#include <cmath>

namespace cs224 {

	template<typename T>
    static constexpr T pow2(T x) { return x*x; }

    template<typename T>
    static constexpr T pow3(T x) { return x*x*x; }

    template<typename T>
    static constexpr T pow6(T x) { return x*x*x*x*x*x; }

    template<typename T>
    static constexpr T pow9(T x) { return x*x*x*x*x*x*x*x*x; }

}