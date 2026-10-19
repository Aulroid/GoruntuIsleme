#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

int main() {
  cv::Mat img = cv::imread("ornek.jpg");
  if (img.empty()) {
    std::cerr << "Goruntu okunamadi!\n";
    return -1;
  }

  std::vector<cv::Mat> channels;
  cv::split(img, channels);  // Görüntüyü B, G, R kanallarına ayır

  int histSize = 256;
  float range[] = {0, 256};
  const float* histRange = {range};
  cv::Mat cvHist;

  for (size_t c = 0; c < channels.size(); ++c) {
    cv::calcHist(&channels[c], 1, 0, cv::Mat(), cvHist, 1, &histSize,
                 &histRange);
    std::cout << "Kanal " << c << " icin OpenCV histogrami cikarildi.\n";
  }

  return 0;
}
