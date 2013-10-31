// cpu_forwards.inl
// Forward declarations for tranform specializations
//

#include "../../transforms/cartographic.hpp"

namespace transform {
	template<>
	void do_op<
		transforms::projection<
			cartographic::projections::latlong,
			cartographic::projections::tmerc<double>,
			cartographic::ellipsoids::sphere>,
		double,
		double
	>(const double& x, const double& y, double& ox, double&oy);
}
