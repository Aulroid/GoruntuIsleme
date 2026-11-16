cat << 'EOF' > hafta9/gorev_5/main.cpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <algorithm>
#include <filesystem>

int main() {
    std::string imgPath = "insan.bmp";

    // Eğer dizinde insan.bmp yoksa test için ten tonları ve arka plan içeren sentetik resim oluştur
    if (!std::filesystem::exists(imgPath)) {
        cv::Mat testImg(400, 600, CV_8UC3, cv::Scalar(180, 120, 50)); // Mavi tonlu arka plan
        cv::ellipse(testImg, cv::Point(300, 200), cv::Size(90, 120), 0, 0, 360, cv::Scalar(135, 165, 220), -1); // Ten rengi (BGR)
        cv::imwrite(imgPath, testImg);
        std::cout << "Not: 'insan.bmp' bulunamadigi icin test goruntusu olusturuldu. Kendi fotografinizi 'insan.bmp' adiyla koyabilirsiniz.\n";
    }

    cv::Mat src = cv::imread(imgPath, cv::IMREAD_COLOR);
    if (src.empty()) {
        std::cerr << "Goruntu yuklenemedi!" << std::endl;
        return -1;
    }

    cv::Mat resRGB = src.clone();
    cv::Mat resYCbCr = src.clone();
    cv::Mat resHSV = src.clone();

    // Tam kırmızı renk (OpenCV BGR formatında: B=0, G=0, R=255)
    const cv::Vec3b RED_COLOR(0, 0, 255);

    // 1. RGB Uzayında Ten Tespiti (Kovac Kuralları)
    for (int r = 0; r < src.rows; ++r) {
        for (int c = 0; c < src.cols; ++c) {
            cv::Vec3b bgr = src.at<cv::Vec3b>(r, c);
            int B = bgr[0], G = bgr[1], R = bgr[2];

            int maxRGB = std::max({R, G, B});
            int minRGB = std::min({R, G, B});

            bool isSkinRGB = (R > 95) && (G > 40) && (B > 20) &&
                             ((maxRGB - minRGB) > 15) &&
                             (std::abs(R - G) > 15) && (R > G) && (R > B);

            if (!isSkinRGB) {
                resRGB.at<cv::Vec3b>(r, c) = RED_COLOR;
            }
        }
    }

    // 2. YCbCr Uzayında Ten Tespiti (OpenCV YCrCb kullanır: Kanal 0=Y, Kanal 1=Cr, Kanal 2=Cb)
    cv::Mat ycrcbImg;
    cv::cvtColor(src, ycrcbImg, cv::COLOR_BGR2YCrCb);

    for (int r = 0; r < ycrcbImg.rows; ++r) {
        for (int c = 0; c < ycrcbImg.cols; ++c) {
            cv::Vec3b ycrcb = ycrcbImg.at<cv::Vec3b>(r, c);
            int Cr = ycrcb[1];
            int Cb = ycrcb[2];

            bool isSkinYCbCr = (Cr >= 133 && Cr <= 173) && (Cb >= 77 && Cb <= 127);

            if (!isSkinYCbCr) {
                resYCbCr.at<cv::Vec3b>(r, c) = RED_COLOR;
            }
        }
    }

    // 3. HSV Uzayında Ten Tespiti (OpenCV H: [0, 179], S: [0, 255], V: [0, 255])
    cv::Mat hsvImg;
    cv::cvtColor(src, hsvImg, cv::COLOR_BGR2HSV);

    for (int r = 0; r < hsvImg.rows; ++r) {
        for (int c = 0; c < hsvImg.cols; ++c) {
            cv::Vec3b hsv = hsvImg.at<cv::Vec3b>(r, c);
            int H = hsv[0];
            int S = hsv[1];
            int V = hsv[2];

            bool isSkinHSV = (H >= 0 && H <= 25) && (S >= 40 && S <= 255) && (V >= 50 && V <= 255);

            if (!isSkinHSV) {
                resHSV.at<cv::Vec3b>(r, c) = RED_COLOR;
            }
        }
    }

    cv::imwrite("ten_tespiti_rgb.bmp", resRGB);
    cv::imwrite("ten_tespiti_ycbcr.bmp", resYCbCr);
    cv::imwrite("ten_tespiti_hsv.bmp", resHSV);

    cv::imshow("Orijinal", src);
    cv::imshow("RGB Ten Tespiti", resRGB);
    cv::imshow("YCbCr Ten Tespiti", resYCbCr);
    cv::imshow("HSV Ten Tespiti", resHSV);

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
EOF