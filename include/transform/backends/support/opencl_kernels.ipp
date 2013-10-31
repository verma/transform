// opencl_kernels.hpp
// opencl kernel loaders foward declaration
//

namespace transform {
	namespace backends {
		namespace detail {
			// kernel for scaling doubles
			//
			template<>
			struct kernel<transforms::scale<double>> {
				static std::pair<cl_program, cl_kernel> load_transform(cl_context ctx, cl_device_id dev);
				static void configure_transform(const transforms::scale<double>& s,
						cl_kernel kernel,
						cl_mem x_in, cl_mem y_in, cl_mem x_out, cl_mem y_out, size_t num_elements);
			};
		}
	}
}
