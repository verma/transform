// opencl_detail.hpp
// method implementations for some of the opencl backend methods.
//

#ifndef __transform_backends_support_opencl_detail_hpp__
#define __transform_backends_support_opencl_detail_hpp__

#include "../../utility.hpp"

#define pad(x,n) (((x + n-1) / n) * n)

namespace transform {
	namespace backends {
		template<typename TDeviceType>
		template<typename TContainer>
		cl_mem opencl<TDeviceType>::make_cl_mem(TContainer& c) const {
			typedef typename TContainer::value_type element_type;

			size_t ps = boost::size(c);
			size_t bs = pad(ps * sizeof(element_type), 1024);

			assert(context_ != NULL);

			int err;
			cl_mem b = clCreateBuffer(context_, CL_MEM_WRITE_ONLY,
					bs, NULL, &err);
			if (!b || err != CL_SUCCESS)
				throw std::runtime_error("Out of memory while trying to allocate OpenCL buffer");

			return b;
		}

		template<typename TDeviceType>
		template<typename TContainer>
		cl_mem opencl<TDeviceType>::make_cl_mem(const TContainer& c, cl_event *evt) const {
			typedef typename TContainer::value_type element_type;

			size_t ps = boost::size(c);
			size_t bs = pad(ps * sizeof(element_type), 1024);

			assert(context_ != NULL);
			assert(queue_ != NULL);

			int err;
			cl_mem b = clCreateBuffer(context_, CL_MEM_READ_ONLY,
					bs, NULL, &err);
			if (!b || err != CL_SUCCESS)
				throw std::runtime_error("Out of memory while trying to allocate OpenCL buffer");

			cl_bool sync = (evt == NULL) ? CL_TRUE : CL_FALSE;

			err = 
				clEnqueueWriteBuffer(queue_, b, sync, 0, sizeof(element_type) * boost::size(c),
						&c[0], 0, NULL, evt);
			if (err != CL_SUCCESS)
				throw std::runtime_error("Failed to queue buffer upload");

			return b;
		}

		template<typename TDeviceType>
		template<typename TContainer>
		void opencl<TDeviceType>::download_to_host(cl_command_queue q, cl_mem mem, TContainer& c, cl_event *evt) const {
			typedef typename TContainer::value_type element_type;

			bool sync = (evt == NULL) ? CL_TRUE : CL_FALSE;
			int err =
				clEnqueueReadBuffer(q, mem, sync, 0, sizeof(element_type) * boost::size(c),
						&c[0], 0, NULL, evt);

			if (err != CL_SUCCESS)
				throw std::runtime_error("Failed to queue download to host");
		}


		template<typename TDeviceType>
		bool opencl<TDeviceType>::supports_double_precision() {
			int err;
			cl_device_id device_id;

			err = clGetDeviceIDs(NULL, TDeviceType::device_type, 1, &device_id, NULL);
			if (err != CL_SUCCESS)
				throw std::runtime_error("Failed to initialze OpenCL device");

			cl_device_fp_config fp_config, required_config = 
				CL_FP_FMA | CL_FP_ROUND_TO_NEAREST | CL_FP_ROUND_TO_ZERO | CL_FP_ROUND_TO_INF | CL_FP_INF_NAN | CL_FP_DENORM;

			clGetDeviceInfo(device_id, CL_DEVICE_DOUBLE_FP_CONFIG, sizeof(fp_config), &fp_config, NULL);

			return (fp_config & required_config) == required_config;
		}

		template<typename TDeviceType>
		template<
			typename TTransform,
			typename ForwardIterableInputRange,
			typename ForwardIterableOutputRange
		>
		void opencl<TDeviceType>::run(const TTransform& p,
			const ForwardIterableInputRange& x,
			const ForwardIterableInputRange& y,
			ForwardIterableOutputRange& out_x,
			ForwardIterableOutputRange& out_y) const {
			int err;
			size_t size = boost::size(x);

			assert(size == boost::size(y));
			assert(boost::size(out_x) == boost::size(y));
			assert(boost::size(out_x) == boost::size(out_y));


#if false
			cl_ulong max_buf;
			err =
				clGetDeviceInfo(device_id_, CL_DEVICE_MAX_MEM_ALLOC_SIZE,
						sizeof(cl_ulong), &max_buf, NULL);
#endif
			cl_event uploads[2];

			auto start = util::timer_start();

			// Apply the kernel
			// 
			cl_mem x_in = make_cl_mem(x, &uploads[0]);
			cl_mem y_in = make_cl_mem(y, &uploads[1]);

			cl_mem x_out = make_cl_mem(out_x);
			cl_mem y_out = make_cl_mem(out_y);

			clWaitForEvents(2, uploads);

			auto end = util::timer_end(start);

			detail::opencl_kernel_wrapper<TDeviceType,TTransform>::configure(context_, p, x_in, y_in, x_out, y_out, size);

			cl_kernel kernel = detail::opencl_kernel_wrapper<TDeviceType,TTransform>::kernel();

			err = clEnqueueNDRangeKernel(queue_, kernel, 1, NULL, &size, NULL, 0, NULL, NULL);
			if (err != CL_SUCCESS) {
				throw std::runtime_error("Failed to execute kernel");
			}

			clFinish(queue_);

			cl_event downloads[2];
			download_to_host(queue_, x_out, out_x, &downloads[0]);
			download_to_host(queue_, y_out, out_y, &downloads[1]);

			// wait for downloads to finish
			clWaitForEvents(2, downloads);

			// release allocated buffers
			clReleaseMemObject(x_in);
			clReleaseMemObject(y_in);
			clReleaseMemObject(x_out);
			clReleaseMemObject(y_out);
		}
	}
}

#endif // __transform_backends_support_opencl_detail_hpp__
