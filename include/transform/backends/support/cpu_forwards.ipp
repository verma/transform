// cpu_forwards.inl
// Forward declarations for tranform specializations
//

#include "../../transforms/cartographic.hpp"

namespace transform {
	template<>
	void do_op<
		transforms::projection<
			cartographic::projections::latlong,
			cartographic::projections::tmerc<cartographic::ellipsoids::sphere, double>>,
		double,
		double
	>( const transforms::projection<
			cartographic::projections::latlong,
			cartographic::projections::tmerc<cartographic::ellipsoids::sphere, double>>& p,
			const double& x, const double& y, double& ox, double&oy);

	template<>
	void do_op<
		transforms::projection<
			cartographic::projections::latlong,
			cartographic::projections::tmerc<cartographic::ellipsoids::WGS84, double>>,
		double,
		double
	>(const transforms::projection<
			cartographic::projections::latlong,
			cartographic::projections::tmerc<cartographic::ellipsoids::WGS84, double>>& p,
			const double& x, const double& y, double& ox, double&oy);
}
