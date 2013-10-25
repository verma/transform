// utility.hpp
// Utility functions
//

#ifndef __transform_utility_hpp__
#define __transform_utility_hpp__

#include <chrono>

namespace transform {
	namespace util {
		template<typename Function>
		long long timeit(Function& f) {
			typedef std::chrono::high_resolution_clock Clock;
			typedef std::chrono::milliseconds milliseconds;

			Clock::time_point t0 = Clock::now();
			f();
			Clock::time_point t1 = Clock::now();

			return std::chrono::duration_cast<milliseconds>(t1 - t0).count();
		}
	}
}

#endif // __transform_utility_hpp__