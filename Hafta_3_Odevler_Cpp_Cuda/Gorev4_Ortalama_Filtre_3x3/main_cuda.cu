#include <cuda_runtime.h>

#include <iostream>

__global__ void meanFilterKernel(unsigned char* d_in, unsigned char* d_out,
                                 int width, int height) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
    int sum = 0;
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        sum += d_in[(y + dy) * width + (x + dx)];
      }
    }
    d_out[y * width + x] = sum / 9;
  }
}
