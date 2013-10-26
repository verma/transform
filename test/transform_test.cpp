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
		transform::backends::full_concurrency_multi_cpu::concurrency() << std::endl;
	std::cout << "Total points: " << set_size << std::endl;
	std::cout << "Preping data..." << std::endl;


	std::vector<double> x_in(set_size), y_in(set_size);
	std::vector<double> x_out(set_size), y_out(set_size);

	std::fill(x_in.begin(), x_in.end(), 1.0);
	std::fill(y_in.begin(), y_in.end(), 2.0);

	transform::transformer<transform::backends::cpu> cpu_trans;
	transform::transformer<transform::backends::full_concurrency_multi_cpu> multi_cpu_trans;

	std::cout << "Running tests..." << std::endl;

	time_this(" cpu", [&]() {
		cpu_trans.run(
			transform::transforms::sine_cosine<double>(),
			x_in, y_in, x_out, y_out
		);
	});

	time_this("mcpu", [&]() {
		multi_cpu_trans.run(
			transform::transforms::sine_cosine<double>(),
			x_in, y_in, x_out, y_out
		);
	});

	return 0;
}
