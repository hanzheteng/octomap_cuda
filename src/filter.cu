#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>
#include <cuda_runtime.h>

#define BLOCK_SIZE 128

__global__ void av3(int n, float *in1, float *in2, float *in3, float *out)
{
	int index = threadIdx.x + blockIdx.x * blockDim.x;
	if (index<n)
	{
		out[index] = (in1[index] + in2[index] + in3[index])/3;
	}
}

float *average3(int num, float *in1, float *in2, float *in3, float *out) 
{
	// Initialization
	float *d_in1, *d_in2, *d_in3, *d_out; 
	int size = num * sizeof(float);

	// Allocate memory for device
	cudaMalloc((void **)&d_in1, size);
	cudaMalloc((void **)&d_in2, size);
	cudaMalloc((void **)&d_in3, size);
	cudaMalloc((void **)&d_out, size);

	// Copy inputs to device
	cudaMemcpy(d_in1, in1, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_in2, in2, size, cudaMemcpyHostToDevice);
	cudaMemcpy(d_in3, in3, size, cudaMemcpyHostToDevice);

	// Launch Kernel
	dim3 DimGrid((num-1)/BLOCK_SIZE+1, 1, 1);
	dim3 DimBlock(BLOCK_SIZE, 1, 1);
	av3<<<DimGrid,DimBlock>>>(num, d_in1, d_in2, d_in3, d_out);

	// Wait for the GPU to finish
	cudaDeviceSynchronize();

	// Copy result back to host and cleanup
	cudaMemcpy(out, d_out, size, cudaMemcpyDeviceToHost);
	cudaFree(d_in1); cudaFree(d_in2); cudaFree(d_in3); cudaFree(d_out);
	return out;
}
