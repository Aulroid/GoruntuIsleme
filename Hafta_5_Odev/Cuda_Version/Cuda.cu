#include <cuda_runtime.h>

#include <iostream>

// Görev 3: CUDA Kernel ile Gri Tonlama
__global__ void bgrToGrayKernel(const unsigned char* d_bgr,
                                unsigned char* d_gray, int width, int height) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    int idx = y * width + x;
    int bgrIdx = idx * 3;

    unsigned char b = d_bgr[bgrIdx];
    unsigned char g = d_bgr[bgrIdx + 1];
    unsigned char r = d_bgr[bgrIdx + 2];

    // 8-bit hassasiyet
    d_gray[idx] = (b * 114 + g * 587 + r * 299 + 500) / 1000;
  }
}

// Görev 2: CUDA Kernel ile Histogram (Çakışmaları atomicAdd ile önlüyoruz)
__global__ void calcHistKernel(const unsigned char* d_channel, int* d_hist,
                               int width, int height) {
  int x = blockIdx.x * blockDim.x + threadIdx.x;
  int y = blockIdx.y * blockDim.y + threadIdx.y;

  if (x < width && y < height) {
    int idx = y * width + x;
    unsigned char val = d_channel[idx];
    atomicAdd(&d_hist[val], 1);
  }
}

// Host fonksiyonu örneği
void runCuda(const unsigned char* h_bgr, unsigned char* h_gray, int width,
             int height) {
  unsigned char *d_bgr, *d_gray;
  int imgSize = width * height;

  cudaMalloc(&d_bgr, imgSize * 3);
  cudaMalloc(&d_gray, imgSize);
  cudaMemcpy(d_bgr, h_bgr, imgSize * 3, cudaMemcpyHostToDevice);

  dim3 threads(16, 16);
  dim3 blocks((width + threads.x - 1) / threads.x,
              (height + threads.y - 1) / threads.y);

  bgrToGrayKernel<<<blocks, threads>>>(d_bgr, d_gray, width, height);

  cudaMemcpy(h_gray, d_gray, imgSize, cudaMemcpyDeviceToHost);

  cudaFree(d_bgr);
  cudaFree(d_gray);
}

int main() {
  // CUDA fonksiyonlarını buradan çağırıp test edebilirsin.
  return 0;
}
