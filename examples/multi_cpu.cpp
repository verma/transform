// multi_cpu.cpp
//


#include "transform.hpp"

#include <iostream>

int main() {
	using namespace transform;
	using namespace transform::backends;
	using namespace transform::transforms;

	// full concurrency is a typdef for multi_cpu<0> which uses all available cores
	//
	transformer<full_concurrency_multi_cpu> t1;

	// Just use one thread to do all the work
	//
	transformer<multi_cpu<1>> t2;

	const size_t SIZE = 100000;
	const double e = 1.0e-10;

	std::vector<double> x_in(SIZE), y_in(SIZE);
	std::vector<double> x_out(SIZE), y_out(SIZE);

	std::fill(x_in.begin(), x_in.end(), 1.0);
	std::fill(y_in.begin(), y_in.end(), 2.0);

	t1.run(scale<double>(10.0), x_in, y_in, x_out, y_out);

	assert(abs(x_out[SIZE-1] - 10.0) < e);
	assert(abs(y_out[SIZE-1] - 20.0) < e);

	t2.run(scale<double>(10.0), x_in, y_in, x_out, y_out);

	assert(abs(x_out[SIZE-1] - 10.0) < e);
	assert(abs(y_out[SIZE-1] - 20.0) < e);

	std::cout << "All good." << std::endl;

	return 0;
}
