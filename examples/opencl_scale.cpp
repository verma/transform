// cpu_scale.cpp
//


#include "transform.hpp"

#include <iostream>

int main() {
	using namespace transform;
	using namespace transform::backends;
	using namespace transform::transforms;

	std::cout << "CPU double-precision support: " <<
		(opencl<cpu_device>::supports_double_precision() ? "YES" : "NO") << std::endl;
	std::cout << "GPU double-precision support: " <<
		(opencl<gpu_device>::supports_double_precision() ? "YES" : "NO") << std::endl;

	transformer<opencl<gpu_device>> t;

	size_t SIZE = 10000000;

	std::vector<double> x_in(SIZE), y_in(SIZE);
	std::vector<double> x_out(SIZE), y_out(SIZE);

	std::fill(x_in.begin(), x_in.end(), 1.0);
	std::fill(y_in.begin(), y_in.end(), 2.0);

	t.run(scale<double>(10.0), x_in, y_in, x_out, y_out);

	double e = 1.0e-10;

	assert(abs(x_out[SIZE-1] - 10.0) < e);
	assert(abs(y_out[SIZE-1] - 20.0) < e);

	std::cout << "All good." << std::endl;

	return 0;
}
