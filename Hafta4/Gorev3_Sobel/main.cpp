#include <opencv2/opencv.hpp>
using namespace cv;

int main() {
  Mat img = imread("test_fotografi.jpg", IMREAD_GRAYSCALE);
  if (img.empty()) return -1;

  Mat blurred;
  GaussianBlur(img, blurred, Size(3, 3), 0);

  Mat sobel_x, sobel_y, sobel_combined;
  Sobel(blurred, sobel_x, CV_32F, 1, 0, 3);
  Sobel(blurred, sobel_y, CV_32F, 0, 1, 3);

  magnitude(sobel_x, sobel_y, sobel_combined);
  normalize(sobel_combined, sobel_combined, 0, 255, NORM_MINMAX, CV_8U);

  imwrite("sonuc_3_sobel_birlestirilmis.jpg", sobel_combined);
  return 0;
}
