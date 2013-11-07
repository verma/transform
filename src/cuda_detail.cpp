// cuda_detail.cpp
//

#include "transform/backends/support/cuda_detail.hpp"
#include "cuda_device.h"

namespace transform {
	namespace backends {
		namespace detail {
			template<>
			void run_cuda_transform(const transforms::scale<double>& s,
					const double *x, const double *y,
					double *x_out, double *y_out, size_t n) {
				run_scale_transform(x, y, x_out, y_out, n, s.s);
			}

			template<>
			void run_cuda_transform(const transforms::projection<
					cartographic::projections::latlong,
					cartographic::projections::tmerc<double>,
					cartographic::ellipsoids::sphere>& s,
					const double *x, const double *y,
					double *x_out, double *y_out, size_t n) {
				run_tmerc_s_transform(x, y, x_out, y_out, n,
						6370997.0, s.to.offset.first, s.to.offset.second);
			}
		}
	}
}

