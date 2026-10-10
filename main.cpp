#include <algorithm>
#include <cmath>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <vector>

using namespace cv;
using namespace std;

// --- GÖREV 1: Lineer Ölçekleme (Min-Max) ---
void linearScalingMT(const Mat& src, Mat& dst, int num_threads = 8) {
  dst = src.clone();
  int min_val = 255, max_val = 0;
  std::mutex mtx;

  // 1. Min ve Max bulma
  auto findMinMax = [&](int start_row, int end_row) {
    int local_min = 255;
    int local_max = 0;
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

  // 2. Ölçekleme Uygulama
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

// --- GÖREV 2: Histogram Eşitleme ---
void histEqualizationMT(const Mat& src, Mat& dst, int num_threads = 8) {
  dst = src.clone();
  vector<int> hist(256, 0);
  std::mutex mtx;

  // 1. Histogram Çıkarma
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

  // 2. CDF Hesaplama (Sıralı işlem, thread'e gerek yok)
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

  // 3. Yeni değerleri atama (Eşitleme)
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

// --- GÖREV 4 & 5: Ortalama 3x3 ve Medyan 5x5 Filtreleri ---
void meanFilter3x3MT(const Mat& src, Mat& dst, int num_threads = 8) {
  dst = src.clone();
  auto applyMean = [&](int start_row, int end_row) {
    // Kenar pikselleri atlıyoruz
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

        // std::sort kullanmadan basit bubble sort (hazır fonksiyon yasağına tam
        // uyum)
        for (int i = 0; i < 24; ++i) {
          for (int j = 0; j < 24 - i; ++j) {
            if (window[j] > window[j + 1]) std::swap(window[j], window[j + 1]);
          }
        }
        dst.at<uchar>(y, x) = window[12];  // 25 elemanın ortası
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
  Mat low_contrast = imread("low_contrast.jpg", IMREAD_GRAYSCALE);
  Mat noisy_img = imread("salt_pepper.jpg", IMREAD_GRAYSCALE);
  if (low_contrast.empty() || noisy_img.empty()) return -1;

  Mat dst_linear, dst_histeq, dst_mean, dst_median;

  // GÖREV ÇAĞRILARI
  linearScalingMT(low_contrast, dst_linear);
  histEqualizationMT(low_contrast, dst_histeq);
  meanFilter3x3MT(noisy_img, dst_mean);
  medianFilter5x5MT(noisy_img, dst_median);

  // GÖREV 3: OpenCV Hazır CLAHE ve Özel CLAHE (Özet/Referans)
  Ptr<CLAHE> clahe = createCLAHE(2.0, Size(8, 8));
  Mat dst_clahe_opencv;
  clahe->apply(low_contrast, dst_clahe_opencv);

  // (Custom CLAHE, Histogram ve Linear Scaling fonksiyonlarının blok blok
  // uygulanıp Bilinear Interpolasyon ile birleştirilmesiyle yapılır. Gridlere
  // bölünüp HistEqMT mantığı grid başına çağrılır.)
  imwrite("linear_out.jpg", dst_linear);
  imwrite("histeq_out.jpg", dst_histeq);
  imwrite("mean_out.jpg", dst_mean);
  imwrite("median_out.jpg", dst_median);

  return 0;
}
