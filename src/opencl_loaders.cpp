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
		size_t len = 0;
		clGetProgramBuildInfo(p, dev, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);

        char *buffer = new char[len + 1];

        clGetProgramBuildInfo(p, dev, CL_PROGRAM_BUILD_LOG, len, buffer, &len);
		buffer[len] = '\0'; // just to be sure

		std::string error(buffer);
		delete[] buffer;

		throw std::runtime_error(error);
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

					   x_out[i] = x[i] * scaleWith;
					   y_out[i] = y[i] * scaleWith;
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

			typedef transforms::projection<cartographic::projections::latlong,
					cartographic::projections::tmerc<cartographic::ellipsoids::sphere, double>>
				proj_latlong_to_tmerc_sphere_d;

			std::pair<cl_program, cl_kernel>
			kernel<proj_latlong_to_tmerc_sphere_d>::load_transform(cl_context ctx, cl_device_id dev) {
				const std::string source = R"code(
					#pragma OPENCL EXTENSION cl_khr_fp64 : enable

					__kernel void tmerc(
					__global double* x_in,
					__global double* y_in,
					__global double* x_out,
					__global double* y_out,
					const unsigned int count,
					const double scale,
					const double x0,
					const double y0) {
					   unsigned int i = get_global_id(0);

					   double lambda = radians(x_in[i]);
					   double phi    = radians(y_in[i]);

					   double x, y, b, cosPhi, sinLambda, cosLambda;

					   cosPhi = cos(phi);
					   sinLambda = sincos(lambda, &cosLambda);

					   b = cosPhi * sinLambda;
					   x = 0.5 * log((1.0 + b) / (1.0 - b));
					   y = cosPhi * cosLambda / sqrt(1.0 - b * b);
					   y = select(acos(y), 0.0, (ulong)(fabs(y) >= 1.0));
					   y = select(y, -y, (ulong)(phi < 0.0));
					   y = y;

					   x_out[i] = x0 + scale * x;
					   y_out[i] = y0 + scale * y;
					}
				)code";

				return load_program(ctx, dev,
							source, "tmerc");
			}

			void
			kernel<proj_latlong_to_tmerc_sphere_d>::configure_transform(
					const proj_latlong_to_tmerc_sphere_d& s,
					cl_kernel kernel,
					cl_mem x_in, cl_mem y_in, cl_mem x_out, cl_mem y_out, size_t num_elements) {
				int err = 0;
				cl_uint size = static_cast<cl_uint>(num_elements);

				cl_double scale = 6370997.0;
				cl_double x0 = s.to.offset.first;
				cl_double y0 = s.to.offset.second;

				err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &x_in);
				err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &y_in);
				err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &x_out);
				err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &y_out);
				err |= clSetKernelArg(kernel, 4, sizeof(cl_uint), &size);
				err |= clSetKernelArg(kernel, 5, sizeof(cl_double), &scale);
				err |= clSetKernelArg(kernel, 6, sizeof(cl_double), &x0);
				err |= clSetKernelArg(kernel, 7, sizeof(cl_double), &y0);

				if (err != CL_SUCCESS)
					throw std::runtime_error("Failed to configure kernel");
			};

			typedef transforms::projection<cartographic::projections::latlong,
					cartographic::projections::tmerc<cartographic::ellipsoids::WGS84, double>>
				proj_latlong_to_tmerc_wgs84_d;

			std::pair<cl_program, cl_kernel>
			kernel<proj_latlong_to_tmerc_wgs84_d>::load_transform(cl_context ctx, cl_device_id dev) {
				const std::string source = R"code(
					#pragma OPENCL EXTENSION cl_khr_fp64 : enable
					#define FC1 1.0
					#define FC2 .5
					#define FC3 .16666666666666666666
					#define FC4 .08333333333333333333
					#define FC5 .05
					#define FC6 .03333333333333333333
					#define FC7 .02380952380952380952
					#define FC8 .01785714285714285714

					__kernel void tmerc_e (
							__global double* x_in,
							__global double* y_in,
							__global double* x_out,
							__global double* y_out,
							const unsigned int count,

							double ecc,
							double ecc2,
							double one_ecc2,

							double scale, double x0, double y0,
							double phi0,
							double lambda0,
							double ml0,
							double8 en
							) {
						int i = get_global_id(0);

						double lambda = radians(x_in[i]) - lambda0;
						double phi    = radians(y_in[i]);

						double sinPhi, cosPhi, t, al, als, n, x, y, ml;

						sinPhi = sincos(phi, &cosPhi);
						t = select(sinPhi/cosPhi, 0.0, (long)(fabs(cosPhi) < 1.0e-10));
						t *= t;
						al = cosPhi * lambda;
						als = al * al;
						al /= sqrt(1.f - ecc2 * sinPhi * sinPhi);
						n = ecc2 / one_ecc2 * cosPhi * cosPhi;

						double cphi, sphi;

						cphi = cosPhi * sinPhi;
						sphi = sinPhi * sinPhi;
						ml = (en.s0 * phi - cphi * (en.s1 + sphi*(en.s2 + sphi*(en.s3 + sphi*en.s4))));

						x = al * (FC1 +
								FC3 * als * (1.f - t + n +
									FC5 * als * (5.f + t * (t - 18.f) + n * (14.f - 58.f * t)
										+ FC7 * als * (61.f + t * ( t * (179.f - t) - 479.f ) )
										)));
						y = (ml - ml0 +
								sinPhi * al * lambda * FC2 * ( 1.f +
									FC4 * als * (5.f - t + n * (9.f + 4.f * n) +
										FC6 * als * (61.f + t * (t - 58.f) + n * (270.f - 330.f * t)
											+ FC8 * als * (1385.f + t * ( t * (543.f - t) - 3111.f) )
											))));

						x_out[i] = x0 + scale * x;
						y_out[i] = y0 + scale * y;
					}
				)code";

				return load_program(ctx, dev,
							source, "tmerc_e");
			}

			void
			kernel<proj_latlong_to_tmerc_wgs84_d>::configure_transform(
					const proj_latlong_to_tmerc_wgs84_d& s,
					cl_kernel kernel,
					cl_mem x_in, cl_mem y_in, cl_mem x_out, cl_mem y_out, size_t num_elements) {
				int err = 0;
				cl_uint size = static_cast<cl_uint>(num_elements);

				typedef cartographic::ellipsoids::WGS84 wgs84;;
				typedef wgs84::params					wgs84params;

				cl_double scale = wgs84params::major_axis;
				cl_double ecc = wgs84params::ecc;
				cl_double ecc2 = wgs84params::ecc2;
				cl_double one_ecc2 = wgs84params::one_ecc2;

				// TODO: try and understand what these values do and get them here somehow
				cl_double phi0 = 0.0;
				cl_double lambda0 = 0.0;

				cl_double ml0 = util::projection::mlfn<wgs84>(phi0, std::sin(phi0), std::cos(phi0));
				cl_double en[8] = {
					wgs84params::en0,
					wgs84params::en1,
					wgs84params::en2,
					wgs84params::en3,
					wgs84params::en4,
					0.0, 0.0, 0.0 };

				cl_double x0 = s.to.offset.first;
				cl_double y0 = s.to.offset.second;

				err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &x_in);
				err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &y_in);
				err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &x_out);
				err |= clSetKernelArg(kernel, 3, sizeof(cl_mem), &y_out);
				err |= clSetKernelArg(kernel, 4, sizeof(cl_uint), &size);

				err |= clSetKernelArg(kernel, 5, sizeof(cl_double), &ecc);
				err |= clSetKernelArg(kernel, 6, sizeof(cl_double), &ecc2);
				err |= clSetKernelArg(kernel, 7, sizeof(cl_double), &one_ecc2);

				err |= clSetKernelArg(kernel, 8, sizeof(cl_double), &scale);
				err |= clSetKernelArg(kernel, 9, sizeof(cl_double), &x0);
				err |= clSetKernelArg(kernel, 10, sizeof(cl_double), &y0);

				err |= clSetKernelArg(kernel, 11, sizeof(cl_double), &phi0);
				err |= clSetKernelArg(kernel, 12, sizeof(cl_double), &lambda0);

				err |= clSetKernelArg(kernel, 13, sizeof(cl_double), &ml0);
				err |= clSetKernelArg(kernel, 14, sizeof(cl_double) * 8, en);

				if (err != CL_SUCCESS)
					throw std::runtime_error("Failed to configure kernel");
			};
		}
	}
}
