// proj_detail.cpp
// Proj detail functions
//

#include "transform/backends/proj.hpp"

namespace transform {
	namespace backends {
		template<>
		void detail::pre_process<cartographic::projections::latlong, double*>
			(const cartographic::projections::latlong& p, double *&x, double *&y, size_t size) {
			// convert all points to radians
			//
			const double to_radian = 0.017453292519943295769236907684886;
			const int stride = 1;

			double *px = x, *py = y;
			for (size_t i = 0 ; i < size ; i ++) {
				*px *= to_radian; px += stride;
				*py *= to_radian; py += stride;
			}
		}
		//
		// specializations for pre and post process operations
		template<>
		void detail::post_process<cartographic::projections::latlong, double*>
			(const cartographic::projections::latlong& p, double *&x, double *&y, size_t size) {
			// convert all points to radians
			//
			const double to_degrees = 57.29577951309314;
			const int stride = 1;

			double *px = x, *py = y;
			for (size_t i = 0 ; i < size ; i ++) {
				*px *= to_degrees; px += stride;
				*py *= to_degrees; py += stride;
			}
		}
	}
}
