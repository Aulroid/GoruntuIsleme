#include <cuda_runtime.h>

#include <iostream>

__global__ void linearScalingKernel(unsigned char* d_in, unsigned char* d_out,
                                    int width, int height, int min_val,
                                    int max_val) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    int idx = y * width + x;
    float scaled = 255.0f * (d_in[idx] - min_val) / (max_val - min_val);
    d_out[idx] =
        (scaled > 255) ? 255 : (scaled < 0 ? 0 : (unsigned char)scaled);
  }
}
