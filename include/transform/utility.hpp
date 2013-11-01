// utility.hpp
// Utility functions
//

#ifndef __transform_utility_hpp__
#define __transform_utility_hpp__

#include <chrono>

namespace transform {
	namespace util {
		template<typename Function>
		static inline long long timeit(Function& f) {
			typedef std::chrono::high_resolution_clock Clock;
			typedef std::chrono::milliseconds milliseconds;

			Clock::time_point t0 = Clock::now();
			f();
			Clock::time_point t1 = Clock::now();

			return std::chrono::duration_cast<milliseconds>(t1 - t0).count();
		}

		static inline std::chrono::high_resolution_clock::time_point timer_start() {
			return std::chrono::high_resolution_clock::now();
		}

		static inline long long timer_end(const std::chrono::high_resolution_clock::time_point& s) {
			return std::chrono::duration_cast<std::chrono::milliseconds>(
					std::chrono::high_resolution_clock::now() - s).count();
		}
	}
}

#endif // __transform_utility_hpp__
