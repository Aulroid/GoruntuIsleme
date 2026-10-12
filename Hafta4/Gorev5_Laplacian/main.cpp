#include <opencv2/opencv.hpp>
using namespace cv;

int main() {
  Mat img = imread("test_fotografi.jpg", IMREAD_GRAYSCALE);
  if (img.empty()) return -1;

  Mat blurred;
  GaussianBlur(img, blurred, Size(3, 3), 0);

  Mat lap_3x3, lap_abs;
  Laplacian(blurred, lap_3x3, CV_32F, 3);

  lap_3x3 = abs(lap_3x3);
  normalize(lap_3x3, lap_abs, 0, 255, NORM_MINMAX, CV_8U);

  imwrite("sonuc_5_laplacian.jpg", lap_abs);
  return 0;
}
