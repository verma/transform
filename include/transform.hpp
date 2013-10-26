// transform.hpp
//

#ifndef __transform_transform_hpp__
#define __transform_transform_hpp__

#include "transform/backends/cpu.hpp"
#include "transform/backends/multi_cpu.hpp"

#if HAVE_PROJ
#include "transform/backends/proj.hpp"
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
		template<
			typename TTransform,
			typename ForwardIterableRange
		>
		void run(const TTransform& transform,
				const ForwardIterableRange& x, const ForwardIterableRange& y,
				ForwardIterableRange& xOut, ForwardIterableRange& yOut) {
			TBackend::run(transform, x, y, xOut, yOut);
		}
	};
}

#endif // __transform_transform_hpp__
