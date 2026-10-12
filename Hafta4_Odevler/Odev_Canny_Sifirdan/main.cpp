#include <iostream>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

// 1. Adım: 5x5 Gauss Filtre kullanarak Gürültü
// Bastırma[span_1](start_span)[span_1](end_span)
Mat apply_gaussian_blur(const Mat& img) {
  Mat blurred;
  GaussianBlur(img, blurred, Size(5, 5), 1.4);
  return blurred;
}

// 2. Adım: Sobel Filtre ile gradyan
// hesaplama[span_2](start_span)[span_2](end_span)
void sobel_gradient(const Mat& img, Mat& mag_img, Mat& angle_img) {
  Mat Kx = (Mat_<float>(3, 3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
  Mat Ky = (Mat_<float>(3, 3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);

  Mat Ix, Iy;
  filter2D(img, Ix, CV_32F, Kx);
  filter2D(img, Iy, CV_32F, Ky);

  // Büyüklük (magnitude) ve Açıyı (angle) hesapla
  cartToPolar(Ix, Iy, mag_img, angle_img, true);
  normalize(mag_img, mag_img, 0, 255, NORM_MINMAX, CV_32F);
}

// 3. Adım: Non-maximum suppression (Kenar
// İnceltme)[span_3](start_span)[span_3](end_span)
Mat non_maximum_suppression(const Mat& mag, const Mat& angle) {
  Mat Z = Mat::zeros(mag.size(), CV_32F);

  for (int i = 1; i < mag.rows - 1; i++) {
    for (int j = 1; j < mag.cols - 1; j++) {
      float q = 255.0f, r = 255.0f;
      float ang = angle.at<float>(i, j);

      // Açıyı 0-180 aralığına getir
      if (ang < 0) ang += 180.0f;

      // Açı 0 (Yatay)
      if ((ang >= 0 && ang < 22.5) || (ang >= 157.5 && ang <= 180)) {
        q = mag.at<float>(i, j + 1);
        r = mag.at<float>(i, j - 1);
      }
      // Açı 45 (Çapraz)
      else if (ang >= 22.5 && ang < 67.5) {
        q = mag.at<float>(i - 1, j + 1);
        r = mag.at<float>(i + 1, j - 1);
      }
      // Açı 90 (Dikey)
      else if (ang >= 67.5 && ang < 112.5) {
        q = mag.at<float>(i - 1, j);
        r = mag.at<float>(i + 1, j);
      }
      // Açı 135 (Diğer Çapraz)
      else if (ang >= 112.5 && ang < 157.5) {
        q = mag.at<float>(i - 1, j - 1);
        r = mag.at<float>(i + 1, j + 1);
      }

      // Piksel değeri komşularından büyükse tut, değilse 0 yap
      if (mag.at<float>(i, j) >= q && mag.at<float>(i, j) >= r) {
        Z.at<float>(i, j) = mag.at<float>(i, j);
      }
    }
  }
  return Z;
}

// 4. Adım: Hysteresis Thresholding (Çift
// Eşikleme)[span_4](start_span)[span_4](end_span)
Mat hysteresis_thresholding(const Mat& img, float low_r = 0.05,
                            float high_r = 0.15) {
  double minV, maxV;
  minMaxLoc(img, &minV, &maxV);

  float high_t = maxV * high_r;
  float low_t = high_t * low_r;

  Mat res = Mat::zeros(img.size(), CV_8U);
  uint8_t weak = 25, strong = 255;

  // Güçlü ve zayıf pikselleri ayır
  for (int i = 0; i < img.rows; i++) {
    for (int j = 0; j < img.cols; j++) {
      float val = img.at<float>(i, j);
      if (val >= high_t)
        res.at<uint8_t>(i, j) = strong;
      else if (val >= low_t && val < high_t)
        res.at<uint8_t>(i, j) = weak;
    }
  }

  // Zayıf piksellerin etrafında (8 yönlü) güçlü piksel varsa onları da güçlü
  // yap, yoksa sil
  for (int i = 1; i < img.rows - 1; i++) {
    for (int j = 1; j < img.cols - 1; j++) {
      if (res.at<uint8_t>(i, j) == weak) {
        if (res.at<uint8_t>(i + 1, j - 1) == strong ||
            res.at<uint8_t>(i + 1, j) == strong ||
            res.at<uint8_t>(i + 1, j + 1) == strong ||
            res.at<uint8_t>(i, j - 1) == strong ||
            res.at<uint8_t>(i, j + 1) == strong ||
            res.at<uint8_t>(i - 1, j - 1) == strong ||
            res.at<uint8_t>(i - 1, j) == strong ||
            res.at<uint8_t>(i - 1, j + 1) == strong) {
          res.at<uint8_t>(i, j) = strong;
        } else {
          res.at<uint8_t>(i, j) = 0;
        }
      }
    }
  }
  return res;
}

int main() {
  Mat img = imread("test_fotografi.jpg", IMREAD_GRAYSCALE);
  if (img.empty()) {
    cout << "Goruntu okunamadi!" << endl;
    return -1;
  }

  // İstenilen adımları sırayla çağır
  Mat blurred = apply_gaussian_blur(img);

  Mat mag, theta;
  sobel_gradient(blurred, mag, theta);

  Mat nms = non_maximum_suppression(mag, theta);

  Mat final_edges = hysteresis_thresholding(nms);

  // Sonucu diske kaydet
  imwrite("odev_custom_canny_sonuc.jpg", final_edges);
  cout << "Canny algoritmasi tamamlandi. 'odev_custom_canny_sonuc.jpg' "
          "olusturuldu."
       << endl;

  return 0;
}
