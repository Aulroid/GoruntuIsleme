#include <opencv2/opencv.hpp>
#include <iostream>
#include <cuda_runtime.h>
#include <cmath>

#define NUM_THETA 180

__global__ void hough_line_kernel(const unsigned char* d_edges, int width, int height,
                                  int* d_accum, int max_rho,
                                  const float* d_cos, const float* d_sin) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height) {
        if (d_edges[y * width + x] > 0) {
            for (int t = 0; t < NUM_THETA; ++t) {
                int rho = __float2int_rn(x * d_cos[t] + y * d_sin[t]) + max_rho;
                atomicAdd(&d_accum[rho * NUM_THETA + t], 1);
            }
        }
    }
}

int main() {
    cv::Mat img = cv::imread("input.jpg");
    if (img.empty()) return -1;

    cv::Mat gray, edges, filtered;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 50, 150);
    cv::medianBlur(edges, filtered, 3);

    int width = filtered.cols;
    int height = filtered.rows;
    int max_rho = static_cast<int>(std::hypot(width, height));
    int num_rho = 2 * max_rho + 1;

    float h_cos[NUM_THETA], h_sin[NUM_THETA];
    for (int t = 0; t < NUM_THETA; ++t) {
        float rad = t * CV_PI / 180.0f;
        h_cos[t] = std::cos(rad);
        h_sin[t] = std::sin(rad);
    }

    // GPU Bellek Tahsisi
    unsigned char* d_edges;
    int* d_accum;
    float *d_cos, *d_sin;

    size_t img_size = width * height * sizeof(unsigned char);
    size_t accum_size = num_rho * NUM_THETA * sizeof(int);

    cudaMalloc(&d_edges, img_size);
    cudaMalloc(&d_accum, accum_size);
    cudaMalloc(&d_cos, NUM_THETA * sizeof(float));
    cudaMalloc(&d_sin, NUM_THETA * sizeof(float));

    cudaMemcpy(d_edges, filtered.data, img_size, cudaMemcpyHostToDevice);
    cudaMemset(d_accum, 0, accum_size);
    cudaMemcpy(d_cos, h_cos, NUM_THETA * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_sin, h_sin, NUM_THETA * sizeof(float), cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(16, 16);
    dim3 blocksPerGrid((width + threadsPerBlock.x - 1) / threadsPerBlock.x,
                       (height + threadsPerBlock.y - 1) / threadsPerBlock.y);

    hough_line_kernel<<<blocksPerGrid, threadsPerBlock>>>(d_edges, width, height, d_accum, max_rho, d_cos, d_sin);
    cudaDeviceSynchronize();

    int* h_accum = (int*)malloc(accum_size);
    cudaMemcpy(h_accum, d_accum, accum_size, cudaMemcpyDeviceToHost);

    // Sonuclari Cizdirme
    cv::Mat result = img.clone();
    int threshold = 100;
    for (int r = 0; r < num_rho; ++r) {
        for (int t = 0; t < NUM_THETA; ++t) {
            if (h_accum[r * NUM_THETA + t] >= threshold) {
                int rho = r - max_rho;
                double a = h_cos[t], b = h_sin[t];
                double x0 = a * rho, y0 = b * rho;
                cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
                cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
                cv::line(result, pt1, pt2, cv::Scalar(0, 255, 0), 2);
            }
        }
    }

    cv::imwrite("output_cuda_hough_lines.jpg", result);

    // Bellek Temizleme
    cudaFree(d_edges);
    cudaFree(d_accum);
    cudaFree(d_cos);
    cudaFree(d_sin);
    free(h_accum);

    std::cout << "CUDA Hough Line Gorevi tamamlandi." << std::endl;
    return 0;
}