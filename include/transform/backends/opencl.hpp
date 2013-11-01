// opencl.hpp
// OpenCL backend
//

#ifndef __transform_backends_opencl_hpp__
#define __transform_backends_opencl_hpp__

#include "../transforms/basic.hpp"
#include "../transforms/cartographic.hpp"

#include <OpenCL/opencl.h>

#include <boost/range.hpp>

#include <utility>
#include <stdexcept>

namespace transform {
	namespace backends {
		namespace detail {
			template<typename T>
			struct kernel {
				static std::pair<cl_program, cl_kernel> load_transform(cl_context ctx) {
					static_assert(sizeof(T) == 0,
							"You need to specialize load_transform for this type");
				}

				static void configure_transform(const T& t,
						cl_kernel kernel,
						cl_mem x_in, cl_mem y_in, cl_mem x_out, cl_mem y_out, size_t numelements) {
					static_assert(sizeof(T) == 0,
							"You need to specialize apply_to_transform for this type");
				}
			};

			template<typename TDeviceType, typename T>
			struct opencl_kernel_wrapper {
				static void load(cl_context ctx, cl_device_id dev) {
					auto p = detail::kernel<T>::load_transform(ctx, dev);

					program() = p.first;
					kernel() = p.second;

					loaded() = true;
				}

				static void configure(cl_context ctx, const T& t,
						cl_mem x_in, cl_mem y_in, cl_mem x_out, cl_mem y_out,
						size_t numelements) {
					assert(loaded());

					detail::kernel<T>::configure_transform(t, kernel(), x_in, y_in, x_out, y_out, numelements);
				}

				static void release() {
					loaded() = false;

					clReleaseProgram(program());
					clReleaseKernel(kernel());
				}

				static cl_program& program() {
					static cl_program p; return p;
				}

				static cl_kernel& kernel() {
					static cl_kernel k; return k;
				}

				static bool& loaded() {
					static bool l; return l;
				}
			};
		}

		struct cpu_device {
			constexpr static cl_device_type device_type = CL_DEVICE_TYPE_CPU;
		};

		struct gpu_device {
			constexpr static cl_device_type device_type = CL_DEVICE_TYPE_GPU;
		};


		template<
			typename TDeviceType
		>
		struct opencl {
			// typdefs for supported transforms
			//
			typedef transforms::scale<double>					scale_double;
			typedef transforms::projection<
				cartographic::projections::latlong,
				cartographic::projections::tmerc<double>,
				cartographic::ellipsoids::sphere>				projections_latlong_tmerc_double_sphere;

			opencl() {
				int err;

				cl_device_id		device_id;
				cl_context			context;
				cl_command_queue	queue;

				// initialize devices
				err = clGetDeviceIDs(NULL, TDeviceType::device_type, 1, &device_id, NULL);
				if (err != CL_SUCCESS)
					throw std::runtime_error("Failed to initialze OpenCL device");

				context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
				if (!context)
					throw std::runtime_error("Failed to initialize OpenCL context");

				queue = clCreateCommandQueue(context, device_id, 0, &err);
				if (!queue) {
					clReleaseContext(context);
					throw std::runtime_error("Failed to intialize OpenCL command queue");
				}

				device_id_ = device_id;
				context_ = context;
				queue_ = queue;

				// load some of our supported Kernels
				detail::opencl_kernel_wrapper<TDeviceType, scale_double>::load(context, device_id);
				detail::opencl_kernel_wrapper<TDeviceType, projections_latlong_tmerc_double_sphere>::load(context, device_id);
			}

			~opencl() {
				// release our loaded kernels
				detail::opencl_kernel_wrapper<TDeviceType, scale_double>::release();
				detail::opencl_kernel_wrapper<TDeviceType, projections_latlong_tmerc_double_sphere>::release();

				clReleaseCommandQueue(queue_);
				clReleaseContext(context_);
			}

			static bool supports_double_precision();

			template<
				typename TTransform,
				typename ForwardIterableInputRange,
				typename ForwardIterableOutputRange
			>
			void run(const TTransform& p,
				const ForwardIterableInputRange& x,
				const ForwardIterableInputRange& y,
				ForwardIterableOutputRange& out_x,
				ForwardIterableOutputRange& out_y) const;

		private:
			template<typename TContainer> cl_mem make_cl_mem(TContainer& c) const;
			template<typename TContainer> cl_mem make_cl_mem(const TContainer& c) const;
			template<typename TContainer> void download_to_host(cl_command_queue q, 
					cl_mem mem, TContainer& c, cl_event *evt) const;

		private:
			cl_device_id device_id_;
			cl_context context_;
			cl_command_queue queue_;
		};
	}
}

#include "support/opencl_detail.hpp"
#include "support/opencl_kernels.ipp"

#endif // __transform_backends_opencl_hpp__

