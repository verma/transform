// cpu.hpp
// CPU backend
//

#ifndef __transform_backends_cpu_hpp__
#define __transform_backends_cpu_hpp__

#include <boost/range.hpp>
#include <cassert>

namespace transform {
	namespace backends {
		struct cpu {
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

				assert(boost::size(x) == boost::size(y));
				assert(boost::size(x) == boost::size(xOut));
				assert(boost::size(xOut) == boost::size(yOut));

				iterator x_out = boost::begin(xOut), y_out = boost::begin(yOut);

				for (const_iterator iterX = boost::begin(x), iterY = boost::begin(y), iterEndX = boost::end(x) ;
						iterX != iterEndX ; ++ iterX, ++iterY) {
							p.op(*iterX, *iterY, *x_out++, *y_out++);
				}
			}
		};
	}
}

#endif // __transform_backends_cpu_hpp__