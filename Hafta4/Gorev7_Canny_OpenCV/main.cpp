#include <chrono>
#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;
using namespace std::chrono;

int main() {
  Mat img = imread("test_fotografi.jpg", IMREAD_GRAYSCALE);
  if (img.empty()) return -1;

  Mat blurred;
  GaussianBlur(img, blurred, Size(3, 3), 0);

  Mat canny_cv2;
  auto start = high_resolution_clock::now();

  Canny(blurred, canny_cv2, 100, 200);

  auto end = high_resolution_clock::now();
  cout << "OpenCV Canny Calisma Zamani: "
       << duration<double>(end - start).count() << " saniye" << endl;

  imwrite("sonuc_7_canny_opencv.jpg", canny_cv2);
  return 0;
}
