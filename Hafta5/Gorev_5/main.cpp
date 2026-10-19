#include <iostream>
#include <opencv2/opencv.hpp>
#include <string>

// Güvenlik standartlarına uygun palet
cv::Mat applySecurityPalette(const cv::Mat& gray) {
  cv::Mat colorized = cv::Mat::zeros(gray.size(), CV_8UC3);
  for (int y = 0; y < gray.rows; ++y) {
    for (int x = 0; x < gray.cols; ++x) {
      uchar val = gray.at<uchar>(y, x);
      // Standart yoğunluk eşikleri
      if (val < 50)
        colorized.at<cv::Vec3b>(y, x) =
            cv::Vec3b(255, 0, 0);  // Mavi: İnorganik/Metaller
      else if (val < 130)
        colorized.at<cv::Vec3b>(y, x) =
            cv::Vec3b(0, 255, 0);  // Yeşil: Karışık/Hafif Metal
      else if (val < 220)
        colorized.at<cv::Vec3b>(y, x) =
            cv::Vec3b(0, 165, 255);  // Turuncu: Organik maddeler
      else
        colorized.at<cv::Vec3b>(y, x) =
            cv::Vec3b(val, val, val);  // Beyaz: Arkaplan
    }
  }
  return colorized;
}

int main() {
  for (int i = 1; i <= 5; ++i) {  // 5 adet görüntüyü döngü ile oku
    std::string filename = "ThreadsRGB/img" + std::to_string(i) + ".jpg";
    cv::Mat img = cv::imread(filename);
    if (img.empty()) continue;

    cv::Mat gray, colorized;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    colorized = applySecurityPalette(gray);

    cv::imwrite("renklendirilmis_" + std::to_string(i) + ".jpg", colorized);
  }
  return 0;
}
