#include <iostream>
#include <mutex>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

using namespace cv;
using namespace std;

void linearScalingMT(const Mat& src, Mat& dst, int num_threads = 8) {
  dst = src.clone();
  int min_val = 255, max_val = 0;
  std::mutex mtx;

  auto findMinMax = [&](int start_row, int end_row) {
    int local_min = 255, local_max = 0;
    for (int y = start_row; y < end_row; ++y) {
      for (int x = 0; x < src.cols; ++x) {
        uchar val = src.at<uchar>(y, x);
        if (val < local_min) local_min = val;
        if (val > local_max) local_max = val;
      }
    }
    std::lock_guard<std::mutex> lock(mtx);
    if (local_min < min_val) min_val = local_min;
    if (local_max > max_val) max_val = local_max;
  };

  vector<thread> threads;
  int chunk = src.rows / num_threads;
  for (int i = 0; i < num_threads; ++i) {
    int start = i * chunk;
    int end = (i == num_threads - 1) ? src.rows : start + chunk;
    threads.emplace_back(findMinMax, start, end);
  }
  for (auto& t : threads) t.join();
  threads.clear();

  auto applyScale = [&](int start_row, int end_row) {
    for (int y = start_row; y < end_row; ++y) {
      for (int x = 0; x < dst.cols; ++x) {
        dst.at<uchar>(y, x) = saturate_cast<uchar>(
            255.0 * (src.at<uchar>(y, x) - min_val) / (max_val - min_val));
      }
    }
  };

  for (int i = 0; i < num_threads; ++i) {
    int start = i * chunk;
    int end = (i == num_threads - 1) ? src.rows : start + chunk;
    threads.emplace_back(applyScale, start, end);
  }
  for (auto& t : threads) t.join();
}

int main() {
  Mat src = imread("low_contrast.jpg", IMREAD_GRAYSCALE);
  if (src.empty()) {
    cout << "Resim bulunamadi!" << endl;
    return -1;
  }
  Mat dst;
  linearScalingMT(src, dst);
  imwrite("linear_out_cpu.jpg", dst);
  cout << "Gorev 1 (CPU) basariyla calisti ve kaydedildi." << endl;
  return 0;
}
