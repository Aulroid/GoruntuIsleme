#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

// Kendi histogram hesaplama fonksiyonumuz
std::vector<int> myCalcHist(const cv::Mat& channel) {
  std::vector<int> hist(256, 0);
  for (int y = 0; y < channel.rows; ++y) {
    for (int x = 0; x < channel.cols; ++x) {
      hist[channel.at<uchar>(y, x)]++;  // Piksel değerini indekse yaz ve artır
    }
  }
  return hist;
}

int main() {
  cv::Mat img = cv::imread("ornek.jpg");
  if (img.empty()) return -1;

  std::vector<cv::Mat> channels;
  cv::split(img, channels);

  for (size_t c = 0; c < channels.size(); ++c) {
    std::vector<int> customHist = myCalcHist(channels[c]);
    std::cout << "Kanal " << c << " icin ozel histogram hesaplandi.\n";
  }

  return 0;
}
