// proj_test.cpp
// proj backend tests
//
#include <boost/test/unit_test.hpp>

#include "transform.hpp"

void gen_latlong_points(std::vector<double>& x, std::vector<double>& y, 
		size_t count) {
	x.resize(count);
	y.resize(count);

	for (size_t i = 0 ; i < count ; i ++) {
		x.at(i)	= 45 * sin(2 * M_PI * i / count);
        y.at(i) = 45 * cos(2 * M_PI * i / count);
	}
}

void tmerc_proj(const std::string& ell,
		std::vector<double>& x,
		std::vector<double>& y) {
	std::string from = std::string("+proj=latlong");
	std::string to = std::string("+proj=tmerc +ellps=") + ell;

	projPJ pj_in = pj_init_plus(from.c_str()),
		   pj_out = pj_init_plus(to.c_str());

	assert(x.size() == y.size());

	assert(pj_in != NULL);
	assert(pj_out != NULL);

	for (size_t i = 0, il = x.size() ; i < il ; i ++) {
#define TO_RADIAN 0.017453292519943295769236907684886;
		
		x.at(i) *= TO_RADIAN;
		y.at(i) *= TO_RADIAN;
	}

	int error = pj_transform(pj_in, pj_out, x.size(), 1,
			&x[0], &y[0], NULL);

	pj_free(pj_in);
	pj_free(pj_out);

	assert(error == 0);
}

void inv_tmerc_proj(const std::string& ell,
		std::vector<double>& x,
		std::vector<double>& y) {
	std::string to = std::string("+proj=latlong");
	std::string from = std::string("+proj=tmerc +ellps=") + ell;

	projPJ pj_in = pj_init_plus(from.c_str()),
		   pj_out = pj_init_plus(to.c_str());

	assert(x.size() == y.size());

	assert(pj_in != NULL);
	assert(pj_out != NULL);

	int error = pj_transform(pj_in, pj_out, x.size(), 1,
			&x[0], &y[0], NULL);

	for (size_t i = 0, il = x.size() ; i < il ; i ++) {
#define TO_DEGS 57.29577951309314

		x.at(i) *= TO_DEGS;
		y.at(i) *= TO_DEGS;
	}


	pj_free(pj_in);
	pj_free(pj_out);

	assert(error == 0);
}

void prep_tmerc(const std::string& ell, size_t point_count,
		std::vector<double>& x, std::vector<double>& y,
		std::vector<double>& std_x, std::vector<double>& std_y) {
	// generate test comparison data
	//
	x.resize(point_count);
	y.resize(point_count);
	std_x.resize(point_count);
	std_y.resize(point_count);

	gen_latlong_points(x, y, point_count);

	std::copy(x.begin(), x.end(), std_x.begin());
	std::copy(y.begin(), y.end(), std_y.begin());

	tmerc_proj(ell, std_x, std_y);
}

void prep_inverse_tmerc(const std::string& ell, size_t point_count,
		std::vector<double>& x, std::vector<double>& y,
		std::vector<double>& std_x, std::vector<double>& std_y) {
	prep_tmerc(ell, point_count, x, y, std_x, std_y);

	x.resize(point_count);
	y.resize(point_count);
	std_x.resize(point_count);
	std_y.resize(point_count);

	gen_latlong_points(x, y, point_count);

	tmerc_proj(ell, x, y);

	std::copy(x.begin(), x.end(), std_x.begin());
	std::copy(y.begin(), y.end(), std_y.begin());

	inv_tmerc_proj(ell, std_x, std_y);
}

BOOST_AUTO_TEST_SUITE(proj_test)

BOOST_AUTO_TEST_CASE(single_cpu_proj_matches)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_tmerc("sphere", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_from;
	typedef projections::tmerc<ellipsoids::sphere, double>	projection_to;

	transformer<proj> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(full_concurrency_proj_matches)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_tmerc("sphere", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_from;
	typedef projections::tmerc<ellipsoids::sphere, double>	projection_to;

	transformer<full_concurrency_proj> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(single_cpu_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_tmerc("sphere", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_from;
	typedef projections::tmerc<ellipsoids::sphere, double>	projection_to;

	transformer<cpu> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(wgs84_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_tmerc("WGS84", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_from;
	typedef projections::tmerc<ellipsoids::WGS84, double>	projection_to;

	transformer<proj> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(wgs84_cpu_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_tmerc("WGS84", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_from;
	typedef projections::tmerc<ellipsoids::WGS84, double>	projection_to;

	transformer<cpu> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(wgs84_opencl_gpu_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_tmerc("WGS84", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_from;
	typedef projections::tmerc<ellipsoids::WGS84, double>	projection_to;

	transformer<opencl<gpu_device>> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(wgs84_opencl_cpu_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_tmerc("WGS84", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_from;
	typedef projections::tmerc<ellipsoids::WGS84, double>	projection_to;

	transformer<opencl<cpu_device>> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(),
				projection_to(projection_to::offset_t(0.0, 0.0))),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(sphere_proj_inv_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_inverse_tmerc("sphere", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_to;
	typedef projections::tmerc<ellipsoids::sphere, double>	projection_from;

	transformer<proj> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(projection_from::offset_t(0.0, 0.0)),
				projection_to()),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(wgs84_proj_inv_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_inverse_tmerc("WGS84", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_to;
	typedef projections::tmerc<ellipsoids::WGS84, double>	projection_from;

	transformer<proj> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(projection_from::offset_t(0.0, 0.0)),
				projection_to()),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(sphere_cpu_inv_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_inverse_tmerc("sphere", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_to;
	typedef projections::tmerc<ellipsoids::sphere, double>	projection_from;

	transformer<cpu> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(projection_from::offset_t(0.0, 0.0)),
				projection_to()),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}

BOOST_AUTO_TEST_CASE(wgs84_cpu_inv_tmerc)
{
	std::vector<double> x, y, std_x, std_y;

	const size_t SIZE = 10000;

	prep_inverse_tmerc("WGS84", SIZE, x, y, std_x, std_y);
	std::vector<double> out_x(SIZE), out_y(SIZE);

	// generate our data
	using namespace transform;
	using namespace transform::transforms;
	using namespace transform::backends;
	using namespace transform::cartographic;

	typedef projections::latlong							projection_to;
	typedef projections::tmerc<ellipsoids::WGS84, double>	projection_from;

	transformer<cpu> t;
	t.run(projection<projection_from, projection_to>(
				projection_from(projection_from::offset_t(0.0, 0.0)),
				projection_to()),
				x, y, out_x, out_y);

	for(size_t i = 0, il = x.size() ; i < il ; i ++) {
		BOOST_CHECK_CLOSE(std_x.at(i), out_x.at(i), 0.00001);
		BOOST_CHECK_CLOSE(std_y.at(i), out_y.at(i), 0.00001);
	}
}


BOOST_AUTO_TEST_SUITE_END()
