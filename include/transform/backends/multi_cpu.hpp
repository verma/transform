// multi_cpu.hpp
// Multithreaded CPU backend
//

#ifndef __transform_backends_multi_cpu_hpp__
#define __transform_backends_multi_cpu_hpp__

#include <thread>
#include <boost/range.hpp>

#include <algorithm>
#include <vector>

#include "../concurrency.hpp"

namespace transform {
	namespace backends {
		template<unsigned MaxConcurrency = 0>
		struct multi_cpu {
			template<
				typename TTransform,
				typename ForwardIterableInputRange,
				typename ForwardIterableOutputRange
			>
			void run(const TTransform& p,
				const ForwardIterableInputRange& x,
				const ForwardIterableInputRange& y,
				ForwardIterableOutputRange& xOut,
				ForwardIterableOutputRange& yOut) const {
				typedef typename boost::range_iterator<ForwardIterableOutputRange>::type iterator;
				typedef typename boost::range_const_iterator<ForwardIterableInputRange>::type const_iterator;

				auto compute = 
					[&p](const_iterator sx, const_iterator ex, const_iterator sy,
							const_iterator ey, iterator ox, iterator oy) {
					for ( ; sx != ex ; ++sx, ++sy) {
						p.op(*sx, *sy, *ox++, *oy++);
					}
				};

				typename boost::range_difference<ForwardIterableInputRange>::type 
					sx = boost::size(x),
					sy = boost::size(y);

				assert(sx == sy);
				assert((size_t)sx == boost::size(xOut));
				assert(boost::size(xOut) == boost::size(yOut));

				const_iterator xb = boost::begin(x),
							   yb = boost::begin(y);

				iterator ox = boost::begin(xOut),
						 oy = boost::begin(yOut);


				utility::scheduler<MaxConcurrency> c;
				unsigned max_threads = c.concurrency();
				size_t per_batch = sx / max_threads;


				for (unsigned i = 0 ; i < max_threads ; i ++) {
					const_iterator xe = xb + per_batch, ye = yb + per_batch;

					c.queue(compute, xb, xe, yb, ye, ox, oy);

					xb += per_batch;
					yb += per_batch;
					ox += per_batch;
					oy += per_batch;
				}

				c.wait();
			}
		};

		typedef multi_cpu<0> full_concurrency_multi_cpu;
		typedef multi_cpu<1> cpu;
	}
}

#include "support/cpu_forwards.ipp"

#endif // __transform_backends_multi_cpu_hpp__
