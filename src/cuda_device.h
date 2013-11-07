// cuda_detail.hpp
// cuda device detail
//
// We are going to keep this strictly C here so
// we can extern stuff out and do what we want with it

#ifndef __transform_backends_cuda_device_h__
#define __transform_backends_cuda_device_h__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int run_scale_transform(const double *x, const double *y, double *x_out, double *y_out,
		size_t n, double scale_with);

int run_tmerc_s_transform(const double *x, const double *y, double *x_out, double *y_out,
		size_t n, double scale, double x0, double y0);

#ifdef __cplusplus
}
#endif

#endif // __transform_backends_cuda_device_h__
