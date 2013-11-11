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

			// kernel for cartographc projection from lat-long to tmerc<double>, with sphere ellipsoid
			//
			template<>
			struct kernel<transforms::projection<
				cartographic::projections::latlong,
				cartographic::projections::tmerc<cartographic::ellipsoids::sphere, double>>> {
				static std::pair<cl_program, cl_kernel> load_transform(cl_context ctx, cl_device_id dev);
				static void configure_transform(
					const transforms::projection<
						cartographic::projections::latlong,
						cartographic::projections::tmerc<cartographic::ellipsoids::sphere, double>>& s,
					cl_kernel kernel,
					cl_mem x_in, cl_mem y_in, cl_mem x_out, cl_mem y_out, size_t num_elements);
			};

			// kernel for cartographc projection from lat-long to tmerc<double>, with WGS84 ellipsoid
			//
			template<>
			struct kernel<transforms::projection<
				cartographic::projections::latlong,
				cartographic::projections::tmerc<cartographic::ellipsoids::WGS84, double>>> {
				static std::pair<cl_program, cl_kernel> load_transform(cl_context ctx, cl_device_id dev);
				static void configure_transform(
					const transforms::projection<
						cartographic::projections::latlong,
						cartographic::projections::tmerc<cartographic::ellipsoids::WGS84, double>>& s,
					cl_kernel kernel,
					cl_mem x_in, cl_mem y_in, cl_mem x_out, cl_mem y_out, size_t num_elements);
			};
		}
	}
}
