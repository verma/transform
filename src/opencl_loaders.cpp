// opencl_loaders.cpp
// Opencl template loader specializations
//


#include "transform/backends/opencl.hpp"

#include <iostream>

// some helper functions to ease up loading source
static std::pair<cl_program, cl_kernel> load_program(
		cl_context ctx, cl_device_id dev,
		const std::string& source, const std::string& kernel_name) {
	const char *psource = source.c_str();
	int err;

	cl_program p =
		clCreateProgramWithSource(ctx, 1, (const char **)&psource, NULL, &err);
	if (!p)
		throw std::runtime_error("Failed to create program from source");

	// try and build the program
	err = clBuildProgram(p, 0, NULL, NULL, NULL, NULL);
	if (err != CL_SUCCESS) {
        char buffer[2048];
		size_t len = sizeof(buffer) - 1;

        clGetProgramBuildInfo(p, dev, CL_PROGRAM_BUILD_LOG, sizeof(buffer), buffer, &len);
		buffer[len] = '\0'; // just to be sure

		throw std::runtime_error(buffer);
	}

	cl_kernel k = clCreateKernel(p, kernel_name.c_str(), &err);
	if (!k || err != CL_SUCCESS) {
		clReleaseProgram(p);
		throw std::runtime_error("Failed to load kernel");
	}

	return std::make_pair(p, k);
}

namespace transform {
	namespace backends {
		namespace detail {
			std::pair<cl_program, cl_kernel>
			kernel<transforms::scale<double>>::load_transform(cl_context ctx, cl_device_id dev) {
				const std::string source = R"code(
					#pragma OPENCL EXTENSION cl_khr_fp64 : enable

					__kernel void scale(
					__global double* x,
					__global double* y,
					__global double* x_out,
					__global double* y_out,
					const unsigned int count,
					const double scaleWith) {
					   int i = get_global_id(0);
					   if(i < count) {
						   x_out[i] = x[i] * scaleWith;
						   y_out[i] = y[i] * scaleWith;
					   }
					}
				)code";

				return load_program(ctx, dev,
							source, "scale");
			}

			void
			kernel<transforms::scale<double>>::configure_transform(
					const transforms::scale<double>& s, 
					cl_kernel kernel,
					cl_mem x_in, cl_mem y_in, cl_mem x_out, cl_mem y_out, size_t num_elements) {
				int err = 0;
				cl_uint size = static_cast<cl_uint>(num_elements);

				// set the parameters
				err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &x_in);
				err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &y_in);
				err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &x_out);
				err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &y_out);
				err |= clSetKernelArg(kernel, 4, sizeof(cl_uint), &size);
				err |= clSetKernelArg(kernel, 5, sizeof(cl_double), &s.s);

				if (err != CL_SUCCESS)
					throw std::runtime_error("Failed to configure kernel");
			}
		}
	}
}
