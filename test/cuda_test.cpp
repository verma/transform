// proj_test.cpp
// proj backend tests
//
#include <boost/test/unit_test.hpp>

#include "transform.hpp"

static void gen_latlong_points(std::vector<double>& x, std::vector<double>& y, 
		size_t count) {
	x.resize(count);
	y.resize(count);

	for (size_t i = 0 ; i < count ; i ++) {
		x.at(i)	= 45 * sin(2 * M_PI * i / count);
        y.at(i) = 45 * cos(2 * M_PI * i / count);
	}
}


BOOST_AUTO_TEST_SUITE(cuda_test)

BOOST_AUTO_TEST_CASE(gpu_device_scales_correctly)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	gen_latlong_points(x, y, SIZE);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	transformer<opencl<gpu_device>> t;
	t.run(scale<double>(110232.0),
			x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(x.at(i) * 110232.0, out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(y.at(i) * 110232.0, out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_SUITE_END()
