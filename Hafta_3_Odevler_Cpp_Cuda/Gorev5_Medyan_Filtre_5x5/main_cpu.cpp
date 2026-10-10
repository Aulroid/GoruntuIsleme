#include <algorithm>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

using namespace cv;
using namespace std;

void medianFilter5x5MT(const Mat& src, Mat& dst, int num_threads = 8) {
  dst = src.clone();
  auto applyMedian = [&](int start_row, int end_row) {
    start_row = std::max(2, start_row);
    end_row = std::min(src.rows - 2, end_row);

    for (int y = start_row; y < end_row; ++y) {
      for (int x = 2; x < src.cols - 2; ++x) {
        vector<uchar> window(25);
        int k = 0;
        for (int dy = -2; dy <= 2; ++dy)
          for (int dx = -2; dx <= 2; ++dx)
            window[k++] = src.at<uchar>(y + dy, x + dx);

        for (int i = 0; i < 24; ++i) {
          for (int j = 0; j < 24 - i; ++j) {
            if (window[j] > window[j + 1]) std::swap(window[j], window[j + 1]);
          }
        }
        dst.at<uchar>(y, x) = window[12];
      }
    }
  };

  vector<thread> threads;
  int chunk = src.rows / num_threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(applyMedian, i * chunk,
                         (i == num_threads - 1) ? src.rows : (i + 1) * chunk);
  }
  for (auto& t : threads) t.join();
}

int main() {
  Mat src = imread("salt_pepper.jpg", IMREAD_GRAYSCALE);
  if (src.empty()) return -1;
  Mat dst;

  // 1. Odevde istenen: Once OpenCV hazir Median Blur (Karsilastirma icin)
  Mat dst_opencv;
  medianBlur(src, dst_opencv, 5);
  imwrite("median_opencv_out.jpg", dst_opencv);

  // 2. Odevde istenen: Hazir fonksiyon kullanmadan kendi yazdigimiz (MT)
  medianFilter5x5MT(src, dst);
  imwrite("median_custom_out_cpu.jpg", dst);

  cout << "Gorev 5 (CPU) basariyla calisti ve kaydedildi." << endl;
  return 0;
}
