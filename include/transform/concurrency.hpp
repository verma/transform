// concurrency_capable.hpp
// base class for some of the backends which allow concurrency capabilities
//

#ifndef __transform_concurrency_hpp__
#define __transform_concurrency_hpp__

#include <thread>
#include <vector>
#include <algorithm>
#include <utility>

namespace transform {
	namespace utility {
		template<unsigned MaxConcurrency = 0>
		class scheduler {
		public:
			template<
				class F,
				class ...Args
			>
			void queue(F&& f, Args&&... args) {
				std::thread t(std::forward<F>(f), std::forward<Args>(args)...);
				threads_.push_back(std::forward<std::thread>(t));
			}

			void wait() {
				std::for_each(threads_.begin(), threads_.end(),
					[](std::thread& t) {
						t.join();
					});
			}

			static unsigned concurrency() {
				unsigned con = std::thread::hardware_concurrency();
				if (MaxConcurrency == 0)
					return con;
				return std::min(MaxConcurrency, con);
			}

		private:
			std::vector<std::thread> threads_;
		};

		template<>
		class scheduler<1> {
		public:
			template<
				class F,
				class... Args
			>
			void queue(F&& f, Args&&... args) {
				f(std::forward<Args>(args)...);
			}

			void wait() { }

			static unsigned concurrency() { return 1; }
		};
	}
}

#endif // __transform_concurrency_hpp__
