#include <opencv2/opencv.hpp>
using namespace cv;

int main() {
  Mat img = imread("test_fotografi.jpg", IMREAD_GRAYSCALE);
  if (img.empty()) return -1;

  Mat blurred_7x7, log_7x7, log_abs;
  GaussianBlur(img, blurred_7x7, Size(7, 7), 0);

  Laplacian(blurred_7x7, log_7x7, CV_32F, 7);

  log_7x7 = abs(log_7x7);
  normalize(log_7x7, log_abs, 0, 255, NORM_MINMAX, CV_8U);

  imwrite("sonuc_6_log_7x7.jpg", log_abs);
  return 0;
}
