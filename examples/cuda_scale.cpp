// cuda_scale.cpp
//


#include "transform.hpp"

#include <iostream>

int main() {
	using namespace transform;
	using namespace transform::backends;
	using namespace transform::transforms;

	transformer<cuda> t;

	std::vector<double> x_in(1000), y_in(1000);
	std::vector<double> x_out(1000), y_out(1000);

	std::fill(x_in.begin(), x_in.end(), 1.0);
	std::fill(y_in.begin(), y_in.end(), 2.0);

	t.run(scale<double>(10.0), x_in, y_in, x_out, y_out);

	double e = 1.0e-10;

	std::cout << x_out[999] << ", " << y_out[999] << std::endl;

	assert(abs(x_out[999] - 10.0) < e);
	assert(abs(y_out[999] - 20.0) < e);


	std::cout << "All Done." << std::endl;

	return 0;
}
