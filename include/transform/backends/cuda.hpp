// cuda.hpp
// CUDA backend
//

#ifndef __transform_backends_cuda_hpp__
#define __transform_backends_cuda_hpp__

#include <boost/range.hpp>
#include <memory>

#include "support/cuda_detail.hpp"

namespace transform {
	namespace backends {
		struct cuda {
			// typdefs for supported transforms
			//
			cuda() { }
			~cuda() { }

			static bool supports_double_precision();

			template<
				typename TTransform,
				typename ForwardIterableInputRange,
				typename ForwardIterableOutputRange
			>
			void run(const TTransform& p,
				const ForwardIterableInputRange& x,
				const ForwardIterableInputRange& y,
				ForwardIterableOutputRange& out_x,
				ForwardIterableOutputRange& out_y) const {
				detail::run_transform(p, x, y, out_x, out_y);
			}
		};
	}
}

#endif // __transform_backends_cuda_hpp__

