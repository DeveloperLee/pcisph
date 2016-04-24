#pragma once

#include <tbb/parallel_for.h>
#include <tbb/enumerable_thread_specific.h>

namespace cs224 {

typedef tbb::enumerable_thread_specific<float> Thread_float;    

namespace ConcurrentUtils{
    
    // Template function that concurrently runs a loop operation
    // which significantly improves the simulation performance 
    // compares to a single-thread loop.
	template<typename Func>
    inline void ccLoop(size_t count, Func func) {
        tbb::parallel_for(0ul, count, 1ul, [func] (size_t i) { func(i); });
    }

} // namespace ConcurrentUtils
} // namespace cs224
