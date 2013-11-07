// cuda_detail.hpp
//

#ifndef __transform_backends_cuda_detail_hpp__
#define __transform_backends_cuda_detail_hpp__

#include "../../transforms/basic.hpp"
#include "../../transforms/cartographic.hpp"

#include <boost/range.hpp>

namespace transform {
	namespace backends {
		namespace detail {

			template<typename TTransform, typename TIn, typename TOut>
			void run_cuda_transform(const TTransform& p,
					const TIn *x, const TIn *y,
					TOut *out_x, TOut *out_y, size_t s) {
				static_assert(sizeof(TTransform) == 0,
						"You need to specialize this function to run a specific cuda kernel");
			}

			template<
				typename TTransform,
				typename ForwardIterableInputRange,
				typename ForwardIterableOutputRange
			>
			void run_transform(const TTransform& p,
				const ForwardIterableInputRange& x,
				const ForwardIterableInputRange& y,
				ForwardIterableOutputRange& out_x,
				ForwardIterableOutputRange& out_y) {

				size_t size = boost::size(x);

				assert(size == boost::size(y));
				assert(boost::size(out_x) == boost::size(y));
				assert(boost::size(out_x) == boost::size(out_y));

				run_cuda_transform(p, &x[0], &y[0], &out_x[0], &out_y[1], size);
			}

			template<>
			void run_cuda_transform(const transforms::scale<double>& s,
					const double *x, const double *y,
					double *x_out, double *y_out, size_t n);

			template<>
			void run_cuda_transform(const transforms::projection<
					cartographic::projections::latlong,
					cartographic::projections::tmerc<double>,
					cartographic::ellipsoids::sphere>& s,
					const double *x, const double *y,
					double *x_out, double *y_out, size_t n);
		}
	}
}

#endif // __transform_backends_cuda_detail_hpp__
