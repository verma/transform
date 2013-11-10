// transform_test.cpp
//

#include "transform.hpp"

#include <iostream>
#include <vector>
#include <string>

template<typename Callable>
void time_this(const std::string& name, Callable c) {
	using transform::util::timeit;

	c(); // warmup
	c(); // warmup
	long long ms = timeit(c);

	std::cout << name << " : " << ms << "ms" << std::endl;
}

int main() {
	size_t set_size = 50000000;

	std::cout << "Maximum concurrency: " <<
		transform::utility::scheduler<>::concurrency() << std::endl;
	std::cout << "Total points: " << set_size << std::endl;
	std::cout << "Preping data..." << std::endl;


	std::vector<double> x(set_size), y(set_size);
	std::vector<double> out_x(set_size), out_y(set_size);

	for (size_t i = 0 ; i < set_size ; i ++) {
		x.at(i)	= 45 * sin(2 * M_PI * i / set_size);
        y.at(i) = 45 * cos(2 * M_PI * i / set_size);
	}

	using namespace transform::backends;
	using namespace transform::transforms;
	using namespace transform::cartographic;

	transform::transformer<proj> proj;
	transform::transformer<full_concurrency_proj> mproj;

	typedef projections::latlong							projection_from;
	typedef projections::tmerc<ellipsoids::sphere, double>	projection_to;

	std::cout << "Running tests..." << std::endl;

	time_this(" proj", [&]() {
		proj.run(
			projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);
	});

	time_this("mproj", [&]() {
		mproj.run(
			projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);
	});

	return 0;
}
