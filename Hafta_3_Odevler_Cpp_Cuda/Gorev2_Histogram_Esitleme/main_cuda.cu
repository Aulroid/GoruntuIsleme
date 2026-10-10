#include <cuda_runtime.h>
#include <math.h>

#include <iostream>

__global__ void calcHistKernel(unsigned char* d_in, int* d_hist, int width,
                               int height) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    int idx = y * width + x;
    atomicAdd(&d_hist[d_in[idx]], 1);
  }
}

__global__ void applyHistEqKernel(unsigned char* d_in, unsigned char* d_out,
                                  int* d_cdf, int total_pixels, int min_cdf,
                                  int width, int height) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    int idx = y * width + x;
    int val = d_in[idx];
    float scaled =
        roundf(255.0f * (d_cdf[val] - min_cdf) / (total_pixels - min_cdf));
    d_out[idx] =
        (scaled > 255) ? 255 : (scaled < 0 ? 0 : (unsigned char)scaled);
  }
}
