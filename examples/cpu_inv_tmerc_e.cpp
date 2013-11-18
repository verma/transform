// cpu_tmerc_e.cpp
//


#include "transform.hpp"

#include <iostream>

int main() {
	using namespace transform;
	using namespace transform::backends;
	using namespace transform::transforms;

	// full concurrency is a typdef for multi_cpu<0> which uses all available cores
	//
	transformer<cpu> t1;

	typedef cartographic::projections::latlong												proj_to;
	typedef cartographic::projections::tmerc<cartographic::ellipsoids::WGS84, double>		proj_from;

	projection<proj_from, proj_to> p(proj_from(proj_from::offset_t(0.0, 0.0)), proj_to());

	std::vector<double> x(1), x_out(1);
	std::vector<double> y(1), y_out(1);

	x[0] = 123456.123456;
	y[0] = 123456.123456;

	t1.run(p, x, y, x_out, y_out);

	std::cout << "Transformed: " << std::fixed << x_out[0] << ", " << y_out[0] << std::endl;

	return 0;
}
