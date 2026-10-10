#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

using namespace cv;
using namespace std;

void meanFilter3x3MT(const Mat& src, Mat& dst, int num_threads = 8) {
  dst = src.clone();
  auto applyMean = [&](int start_row, int end_row) {
    start_row = std::max(1, start_row);
    end_row = std::min(src.rows - 1, end_row);

    for (int y = start_row; y < end_row; ++y) {
      for (int x = 1; x < src.cols - 1; ++x) {
        int sum = 0;
        for (int dy = -1; dy <= 1; ++dy)
          for (int dx = -1; dx <= 1; ++dx) sum += src.at<uchar>(y + dy, x + dx);
        dst.at<uchar>(y, x) = sum / 9;
      }
    }
  };

  vector<thread> threads;
  int chunk = src.rows / num_threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(applyMean, i * chunk,
                         (i == num_threads - 1) ? src.rows : (i + 1) * chunk);
  }
  for (auto& t : threads) t.join();
}

int main() {
  Mat src = imread("salt_pepper.jpg", IMREAD_GRAYSCALE);
  if (src.empty()) return -1;
  Mat dst;
  meanFilter3x3MT(src, dst);
  imwrite("mean_out_cpu.jpg", dst);
  cout << "Gorev 4 (CPU) basariyla calisti ve kaydedildi." << endl;
  return 0;
}
