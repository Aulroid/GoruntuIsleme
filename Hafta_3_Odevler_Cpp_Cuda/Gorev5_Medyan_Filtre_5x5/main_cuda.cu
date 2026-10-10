#include <cuda_runtime.h>

#include <iostream>

__global__ void medianFilterKernel(unsigned char* d_in, unsigned char* d_out,
                                   int width, int height) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x >= 2 && x < width - 2 && y >= 2 && y < height - 2) {
    unsigned char window[25];
    int k = 0;
    for (int dy = -2; dy <= 2; dy++) {
      for (int dx = -2; dx <= 2; dx++) {
        window[k++] = d_in[(y + dy) * width + (x + dx)];
      }
    }

    for (int i = 0; i < 24; i++) {
      for (int j = 0; j < 24 - i; j++) {
        if (window[j] > window[j + 1]) {
          unsigned char temp = window[j];
          window[j] = window[j + 1];
          window[j + 1] = temp;
        }
      }
    }
    d_out[y * width + x] = window[12];
  }
}
