cat << 'EOF' > hafta9/gorev_4/main.cpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <filesystem>

// Manuel RGB (BGR) -> HSI dönüşüm fonksiyonu
cv::Mat bgrToHsi(const cv::Mat& bgr) {
    cv::Mat hsi(bgr.rows, bgr.cols, CV_8UC3);
    const double PI = 3.14159265358979323846;

    for (int r = 0; r < bgr.rows; ++r) {
        for (int c = 0; c < bgr.cols; ++c) {
            cv::Vec3b pixel = bgr.at<cv::Vec3b>(r, c);
            double B = pixel[0] / 255.0;
            double G = pixel[1] / 255.0;
            double R = pixel[2] / 255.0;

            double sum = R + G + B;
            double I = sum / 3.0; // Intensity [0, 1]

            double S = 0.0;
            double H = 0.0;

            if (sum > 0.0) {
                double minVal = std::min({R, G, B});
                S = 1.0 - (3.0 / sum) * minVal; // Saturation [0, 1]
            }

            if (S > 0.0) {
                double num = 0.5 * ((R - G) + (R - B));
                double den = std::sqrt((R - G) * (R - G) + (R - B) * (G - B)) + 1e-6;
                double theta = std::acos(std::clamp(num / den, -1.0, 1.0));

                if (B <= G) {
                    H = theta;
                } else {
                    H = 2.0 * PI - theta;
                }
            }

            // 8-bit [0, 255] aralığına ölçekleme
            uchar hByte = static_cast<uchar>((H / (2.0 * PI)) * 255.0);
            uchar sByte = static_cast<uchar>(S * 255.0);
            uchar iByte = static_cast<uchar>(I * 255.0);

            hsi.at<cv::Vec3b>(r, c) = cv::Vec3b(hByte, sByte, iByte);
        }
    }
    return hsi;
}

int main() {
    std::string inputPath = "ornek_rgb.bmp";

    // Test resmi yoksa örnek renkli bir BMP oluştur
    if (!std::filesystem::exists(inputPath)) {
        cv::Mat sample(500, 500, CV_8UC3, cv::Scalar(40, 120, 220));
        cv::circle(sample, cv::Point(250, 250), 150, cv::Scalar(200, 50, 80), -1);
        cv::imwrite(inputPath, sample);
    }

    cv::Mat bgrImg = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (bgrImg.empty()) {
        std::cerr << "Goruntu acilamadi!" << std::endl;
        return -1;
    }

    cv::Mat hsvImg, yuvImg, labImg, hsiImg;

    // Dönüşümler
    cv::cvtColor(bgrImg, hsvImg, cv::COLOR_BGR2HSV);
    cv::cvtColor(bgrImg, yuvImg, cv::COLOR_BGR2YUV);
    cv::cvtColor(bgrImg, labImg, cv::COLOR_BGR2Lab);
    hsiImg = bgrToHsi(bgrImg);

    // BMP formatında kaydetme
    cv::imwrite("cikti_hsv.bmp", hsvImg);
    cv::imwrite("cikti_hsi.bmp", hsiImg);
    cv::imwrite("cikti_yuv.bmp", yuvImg);
    cv::imwrite("cikti_cielab.bmp", labImg);

    std::cout << "Tum renk uzayi donusumleri basariyla BMP formatinda kaydedildi.\n";
    return 0;
}
EOF