#include <iostream>
#include <opencv2/opencv.hpp>

// Fonksiyon pointer tanımı
typedef void (*ColorConversionFunc)(const cv::Mat&, cv::Mat&);

void myBgrToRgb(const cv::Mat& src, cv::Mat& dst) {
  dst = cv::Mat(src.rows, src.cols, CV_8UC3);
  for (int y = 0; y < src.rows; ++y) {
    for (int x = 0; x < src.cols; ++x) {
      cv::Vec3b p = src.at<cv::Vec3b>(y, x);
      dst.at<cv::Vec3b>(y, x) =
          cv::Vec3b(p[2], p[1], p[0]);  // R ve B yer değişti
    }
  }
}

int main() {
  cv::Mat img = cv::imread("ornek.jpg");
  if (img.empty()) return -1;

  // Fonksiyon pointerı ataması
  ColorConversionFunc funcPtr = myBgrToRgb;

  cv::Mat customColor, cvColor;
  funcPtr(img, customColor);  // Pointer üzerinden çağırma
  cv::cvtColor(img, cvColor, cv::COLOR_BGR2RGB);

  cv::Mat diff;
  cv::absdiff(cvColor, customColor, diff);
  cv::Scalar error = cv::sum(diff);

  std::cout << "Görev 4 - Renk Donusum Hata Degeri: "
            << error[0] + error[1] + error[2] << "\n";  // Çıktı: 0 olmalı
  return 0;
}
