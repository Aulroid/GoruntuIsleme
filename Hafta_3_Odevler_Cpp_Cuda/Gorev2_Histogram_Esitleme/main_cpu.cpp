#include <cmath>
#include <iostream>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

using namespace cv;
using namespace std;

void histEqualizationMT(const Mat& src, Mat& dst, int num_threads = 8) {
  dst = src.clone();
  vector<int> hist(256, 0);
  std::mutex mtx;

  auto calcHist = [&](int start_row, int end_row) {
    vector<int> local_hist(256, 0);
    for (int y = start_row; y < end_row; ++y) {
      for (int x = 0; x < src.cols; ++x) {
        local_hist[src.at<uchar>(y, x)]++;
      }
    }
    std::lock_guard<std::mutex> lock(mtx);
    for (int i = 0; i < 256; ++i) hist[i] += local_hist[i];
  };

  vector<thread> threads;
  int chunk = src.rows / num_threads;
  for (int i = 0; i < num_threads; ++i) {
    int start = i * chunk;
    int end = (i == num_threads - 1) ? src.rows : start + chunk;
    threads.emplace_back(calcHist, start, end);
  }
  for (auto& t : threads) t.join();
  threads.clear();

  vector<int> cdf(256, 0);
  cdf[0] = hist[0];
  for (int i = 1; i < 256; ++i) cdf[i] = cdf[i - 1] + hist[i];

  int cdf_min = 0;
  for (int i = 0; i < 256; i++) {
    if (cdf[i] > 0) {
      cdf_min = cdf[i];
      break;
    }
  }
  int total_pixels = src.rows * src.cols;

  auto applyHistEq = [&](int start_row, int end_row) {
    for (int y = start_row; y < end_row; ++y) {
      for (int x = 0; x < dst.cols; ++x) {
        int val = src.at<uchar>(y, x);
        dst.at<uchar>(y, x) = saturate_cast<uchar>(round(
            (double)(cdf[val] - cdf_min) / (total_pixels - cdf_min) * 255.0));
      }
    }
  };

  for (int i = 0; i < num_threads; ++i) {
    int start = i * chunk;
    int end = (i == num_threads - 1) ? src.rows : start + chunk;
    threads.emplace_back(applyHistEq, start, end);
  }
  for (auto& t : threads) t.join();
}

int main() {
  Mat src = imread("low_contrast.jpg", IMREAD_GRAYSCALE);
  if (src.empty()) return -1;
  Mat dst;
  histEqualizationMT(src, dst);
  imwrite("histeq_out_cpu.jpg", dst);
  cout << "Gorev 2 (CPU) basariyla calisti ve kaydedildi." << endl;
  return 0;
}
