#include <opencv2/opencv.hpp>
using namespace cv;

int main() {
  Mat img = imread("test_fotografi.jpg", IMREAD_GRAYSCALE);
  if (img.empty()) return -1;

  Mat blurred;
  GaussianBlur(img, blurred, Size(3, 3), 0);

  Mat prewitt_y, prewitt_abs;
  Mat kernel_y = (Mat_<float>(3, 3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
  filter2D(blurred, prewitt_y, CV_32F, kernel_y);

  prewitt_y = abs(prewitt_y);
  normalize(prewitt_y, prewitt_abs, 0, 255, NORM_MINMAX, CV_8U);

  imwrite("sonuc_2_prewitt_yatay.jpg", prewitt_abs);
  return 0;
}
