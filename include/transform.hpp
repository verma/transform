// transform.hpp
//

#ifndef __transform_transform_hpp__
#define __transform_transform_hpp__

#include "transform/backends/multi_cpu.hpp"
#include "transform/backends/opencl.hpp"

#if HAVE_PROJ4
#include "transform/backends/proj.hpp"
#endif

#if HAVE_CUDA
#include "transform/backends/cuda.hpp"
#endif

#include "transform/transforms/basic.hpp"
#include "transform/transforms/cartographic.hpp"
#include "transform/utility.hpp"


namespace transform {
	template<
		typename TBackend
	>
	class transformer {
		public:
		transformer(): b_() { }

		template<
			typename TTransform,
			typename ForwardIterableInputRange,
			typename ForwardIterableOutputRange
		>
		void run(const TTransform& transform,
				const ForwardIterableInputRange& x, const ForwardIterableInputRange& y,
				ForwardIterableOutputRange& xOut, ForwardIterableOutputRange& yOut) {
			b_.run(transform, x, y, xOut, yOut);
		}

		private:
		TBackend b_;
	};
}

#endif // __transform_transform_hpp__
