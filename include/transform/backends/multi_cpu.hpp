// multi_cpu.hpp
// Multithreaded CPU backend
//

#ifndef __transform_backends_multi_cpu_hpp__
#define __transform_backends_multi_cpu_hpp__

#include <thread>
#include <boost/range.hpp>

#include <algorithm>
#include <vector>

namespace transform {
	namespace backends {
		template<unsigned MaxConcurrency = 0>
		struct multi_cpu {
			template<
				typename TTransform,
				typename ForwardIterableRange
			>
			static void run(const TTransform& p,
				const ForwardIterableRange& x,
				const ForwardIterableRange& y,
				ForwardIterableRange& xOut,
				ForwardIterableRange& yOut) {
				typedef typename boost::range_iterator<ForwardIterableRange>::type iterator;
				typedef typename boost::range_const_iterator<ForwardIterableRange>::type const_iterator;

				auto compute = 
					[&p](const_iterator sx, const_iterator ex, const_iterator sy,
							const_iterator ey, iterator ox, iterator oy) {
					for ( ; sx != ex ; ++sx, ++sy) {
						p.op(*sx, *sy, *ox++, *oy++);
					}
				};

				typename boost::range_difference<ForwardIterableRange>::type 
					sx = boost::size(x),
					sy = boost::size(y);

				assert(sx == sy);
				assert((size_t)sx == boost::size(xOut));
				assert(boost::size(xOut) == boost::size(yOut));

				const_iterator xb = boost::begin(x),
							   yb = boost::begin(y);

				iterator ox = boost::begin(xOut),
						 oy = boost::begin(yOut);

				std::vector<std::thread> threads;
				unsigned max_threads = concurrency();
				size_t per_batch = sx / max_threads;

				for (unsigned i = 0 ; i < max_threads ; i ++) {
					const_iterator xe = xb + per_batch, ye = yb + per_batch;

					std::thread t(compute, xb, xe, yb, ye, ox, oy);
					threads.push_back(std::move(t));

					xb += per_batch;
					yb += per_batch;
					ox += per_batch;
					oy += per_batch;
				}

				std::for_each(threads.begin(), threads.end(), [](std::thread& t) {
					t.join();
				});
			}

			static unsigned concurrency() {
				unsigned con = std::thread::hardware_concurrency();
				if (MaxConcurrency)
					return std::min(MaxConcurrency, con);

				return con;
			}
		};

		typedef multi_cpu<0> full_concurrency_multi_cpu;
	}
}

#endif // __transform_backends_multi_cpu_hpp__
