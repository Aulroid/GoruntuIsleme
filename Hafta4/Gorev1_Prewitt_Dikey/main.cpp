#include <opencv2/opencv.hpp>
using namespace cv;

int main() {
  Mat img = imread("test_fotografi.jpg", IMREAD_GRAYSCALE);
  if (img.empty()) return -1;

  Mat blurred;
  GaussianBlur(img, blurred, Size(3, 3), 0);

  Mat prewitt_x, prewitt_abs;
  Mat kernel_x = (Mat_<float>(3, 3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
  filter2D(blurred, prewitt_x, CV_32F, kernel_x);

  prewitt_x = abs(prewitt_x);
  normalize(prewitt_x, prewitt_abs, 0, 255, NORM_MINMAX, CV_8U);

  imwrite("sonuc_1_prewitt_dikey.jpg", prewitt_abs);
  return 0;
}
