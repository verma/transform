// opencl_marks.cpp
// Compare performance of several backends

#include "transform.hpp"

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

template<typename Callable>
long long time_this(Callable c) {
	using transform::util::timeit;

	c(); // warmup
	c(); // warmup
	long long ms = timeit(c);

	return ms;
}

int main() {
	using namespace transform::backends;
	using namespace transform::transforms;
	using namespace transform::cartographic;


	std::cout << "Maximum CPU concurrency: " <<
		transform::utility::scheduler<>::concurrency() << std::endl;
	std::cout << "CPU double-precision support: " <<
		(opencl<cpu_device>::supports_double_precision() ? "YES" : "NO") << std::endl;
	std::cout << "GPU double-precision support: " <<
		(opencl<gpu_device>::supports_double_precision() ? "YES" : "NO") << std::endl;

	std::cout << std::setw(10) << "#"
		<< std::setw(15) << "count(mil)"
		<< std::setw(10) << "proj"
		<< std::setw(10) << "mproj"
		<< std::setw(10) << "cpu"
		<< std::setw(10) << "mcpu"
		<< std::setw(10) << "cCL"
		<< std::setw(10) << "gCL" 
		<< std::setw(10) << "CUDA"
		<< std::endl;

	size_t base = 1000000;

	for (int i = 1 ; i <= 5 ; i ++) {
		size_t size = base * i * 10;

		std::vector<double> x(size), y(size);
		std::vector<double> out_x(size), out_y(size);

		for (size_t i = 0 ; i < size ; i ++) {
			x.at(i)	= 45 * sin(2 * M_PI * i / size);
			y.at(i) = 45 * cos(2 * M_PI * i / size);
		}


		transform::transformer<proj> proj;
		transform::transformer<full_concurrency_proj> mproj;
		transform::transformer<cpu> cpu;
		transform::transformer<full_concurrency_multi_cpu> mcpu;
		transform::transformer<opencl<cpu_device>> ccl;
		transform::transformer<opencl<gpu_device>> gcl;
		transform::transformer<cuda> cda;


		typedef projections::latlong		projection_from;
		typedef projections::tmerc<double>	projection_to;

		long long t_proj, t_mproj, t_cpu, t_mcpu, t_ccl, t_gcl, t_cuda;

		t_proj =
			time_this([&]() {
				proj.run(
					projection<projection_from, projection_to, ellipsoids::sphere>(
						projection_from(),
						projection_to(projection_to::offset_t(0.0, 0.0))),
					x, y, out_x, out_y);
			});

		t_mproj =
			time_this([&]() {
				mproj.run(
					projection<projection_from, projection_to, ellipsoids::sphere>(
						projection_from(),
						projection_to(projection_to::offset_t(0.0, 0.0))),
					x, y, out_x, out_y);
			});

		t_cpu =
			time_this([&]() {
				cpu.run(
					projection<projection_from, projection_to, ellipsoids::sphere>(
						projection_from(),
						projection_to(projection_to::offset_t(0.0, 0.0))),
					x, y, out_x, out_y);
			});

		t_mcpu =
			time_this([&]() {
				mcpu.run(
					projection<projection_from, projection_to, ellipsoids::sphere>(
						projection_from(),
						projection_to(projection_to::offset_t(0.0, 0.0))),
					x, y, out_x, out_y);
			});

		t_ccl =
			time_this([&]() {
				ccl.run(
					projection<projection_from, projection_to, ellipsoids::sphere>(
						projection_from(),
						projection_to(projection_to::offset_t(0.0, 0.0))),
					x, y, out_x, out_y);
			});

		t_gcl =
			time_this([&]() {
				gcl.run(
					projection<projection_from, projection_to, ellipsoids::sphere>(
						projection_from(),
						projection_to(projection_to::offset_t(0.0, 0.0))),
					x, y, out_x, out_y);
			});

		t_cuda =
			time_this([&]() {
				cda.run(
					projection<projection_from, projection_to, ellipsoids::sphere>(
						projection_from(),
						projection_to(projection_to::offset_t(0.0, 0.0))),
					x, y, out_x, out_y);
			});

	std::cout << std::setw(10) << i
		<< std::setw(15) << size / base
		<< std::setw(10) << t_proj
		<< std::setw(10) << t_mproj
		<< std::setw(10) << t_cpu
		<< std::setw(10) << t_mcpu
		<< std::setw(10) << t_ccl
		<< std::setw(10) << t_gcl
		<< std::setw(10) << t_cuda << std::endl;
	}

	std::cout << "Done!" << std::endl;

	return 0;
}
