#include <iostream>
#include <opencv2/opencv.hpp>

// Hatasız gri tonlama fonksiyonumuz
void myCvtColorToGray(const cv::Mat& src, cv::Mat& dst) {
  dst = cv::Mat(src.rows, src.cols, CV_8UC1);
  for (int y = 0; y < src.rows; ++y) {
    for (int x = 0; x < src.cols; ++x) {
      cv::Vec3b pixel = src.at<cv::Vec3b>(y, x);
      int b = pixel[0], g = pixel[1], r = pixel[2];
      // OpenCV hassasiyeti ile birebir aynı sonuç için:
      dst.at<uchar>(y, x) = (114 * b + 587 * g + 299 * r + 500) / 1000;
    }
  }
}

int main() {
  cv::Mat img = cv::imread("ornek.jpg");
  if (img.empty()) return -1;

  cv::Mat cvGray, customGray;
  cv::cvtColor(img, cvGray, cv::COLOR_BGR2GRAY);
  myCvtColorToGray(img, customGray);

  cv::Mat diffGray;
  cv::absdiff(cvGray, customGray, diffGray);  // Mutlak fark
  cv::Scalar grayError = cv::sum(diffGray);   // Hata toplamı

  std::cout << "Görev 3 - Gri Tonlama Hata Degeri: " << grayError[0]
            << "\n";  // Çıktı: 0 olmalı
  return 0;
}
