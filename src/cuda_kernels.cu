// cuda_kernels.cu
// The cuda device implementation and kernels reside here
//

#include "cuda_device.h"

#include <cuda.h>

#include <stdio.h>

#define CHECK_ERR do { if(err != cudaSuccess) { \
	printf("CUDA call failed: %s(%d): %s\n", __FILE__, __LINE__, cudaGetErrorString(err)); \
	return 0; } } while(0)

// scale kerne that executes on the CUDA device
__global__ void scale_d(double *x_in, double *y_in, int N, double scale) {
	int idx = blockIdx.x * blockDim.x + threadIdx.x;
	if (idx<N) {
		x_in[idx] = x_in[idx] * scale;
		y_in[idx] = y_in[idx] * scale;
	}
}

__global__ void tmerc_sd(double *x_d, double *y_d, int N, double scale, double x0, double y0) {
	int i = threadIdx.x + blockDim.x*blockIdx.x;

	if(i < N) {
		double lambda = x_d[i] * 0.017453292519943295769236907684886;
		double phi    = y_d[i] * 0.017453292519943295769236907684886;

		double x, y, b, cosPhi, sinLambda, cosLambda;

		cosPhi = cos(phi);
		sinLambda = sin(lambda);
		cosLambda = cos(lambda);

		b = cosPhi * sinLambda;
		x = 0.5 * log((1.0 + b) / (1.0 - b));
		y = cosPhi * cosLambda / sqrt(1.0 - b * b);
		if (fabs(y) >= 1.0)
			y = 0.0;
		else
			y = acos(y);

		if (phi < 0.0) y = -y;

		x_d[i] = x0 + scale * x;
		y_d[i] = y0 + scale * y;
	}
}


static int 
prep_buffers(const double *x_in, const double *y_in, size_t size, double **x_d, double **y_d) {
	cudaError_t err;

	err = cudaMalloc(x_d, size * sizeof(double)); CHECK_ERR;
	err = cudaMalloc(y_d, size * sizeof(double)); CHECK_ERR;

	err = cudaMemcpy(*x_d, x_in, size * sizeof(double), cudaMemcpyHostToDevice); CHECK_ERR;
	err = cudaMemcpy(*y_d, y_in, size * sizeof(double), cudaMemcpyHostToDevice); CHECK_ERR;

	return 1;
}

static int
unprep_buffers(double *x_d, double *y_d, size_t size, double *x, double *y) {
	cudaError_t err;

	err = cudaMemcpy(x, x_d, size * sizeof(double), cudaMemcpyDeviceToHost); CHECK_ERR;
	err = cudaMemcpy(y, y_d, size * sizeof(double), cudaMemcpyDeviceToHost); CHECK_ERR;

	cudaFree(x_d);
	cudaFree(y_d);

	return 1;
}

int run_scale_transform(const double *x, const double *y, double *x_out, double *y_out,
		size_t n, double scale_with) {
	double *x_d, *y_d;

	if (!prep_buffers(x, y, n, &x_d, &y_d))
		return 1;


	int block_dim = 512;
	int block_count = (n + block_dim - 1) / block_dim;

	scale_d <<<block_count, block_dim>>>(x_d, y_d, (int)n, scale_with);
	cudaError_t err = cudaPeekAtLastError(); CHECK_ERR;

	unprep_buffers(x_d, y_d, n, x_out, y_out);

	return 0;
}

int run_tmerc_s_transform(const double *x, const double *y, double *x_out, double *y_out,
		size_t n, double scale, double x0, double y0) {
	double *x_d, *y_d;

	if (!prep_buffers(x, y, n, &x_d, &y_d))
		return 1;

	int block_dim = 512;
	int block_count = (n + block_dim - 1) / block_dim;

	tmerc_sd <<<block_count, block_dim>>>(x_d, y_d, (int)n, 6370997.0, 0.0, 0.0);

	unprep_buffers(x_d, y_d, n, x_out, y_out);
	
	return 0;
}
