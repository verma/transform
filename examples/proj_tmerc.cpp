// proj_tmerc.cpp
// tmerc projection using proj backend
//

#include "transform.hpp"

int main() {
	using namespace transform;
	using namespace transform::backends;
	using namespace transform::transforms;
	using namespace transform::cartographic;

	transformer<proj> t;

	const size_t SIZE = 10000;

	std::vector<double> 
		x(SIZE),
		y(SIZE),
		out_x(SIZE),
		out_y(SIZE);

	for (size_t i = 0 ; i < SIZE ; i ++) {
		x.at(i) = 45 * sin(2 * M_PI * i / SIZE);
        y.at(i) = 45 * cos(2 * M_PI * i / SIZE);
	}

	// run latlong -> tmerc projection using the proj.4 backend, with sphere ellipsoid
	// and offsetted at 0.0, 0.0
	
	// give nicer names
	typedef projections::latlong		projection_from;
	typedef projections::tmerc<double>	projection_to;

	t.run(projection<projection_from, projection_to, ellipsoids::sphere>(
				projection_from(), projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);

	return 0;
}
