#include <iostream>
#include <cmath>
#include <opencv2/opencv.hpp>

cv::Mat computeDFT2D(const cv::Mat& inputImg) {
    cv::Mat padded;
    int m = cv::getOptimalDFTSize(inputImg.rows);
    int n = cv::getOptimalDFTSize(inputImg.cols);
    cv::copyMakeBorder(inputImg, padded, 0, m - inputImg.rows, 0, n - inputImg.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
    cv::Mat complexImg;
    cv::merge(planes, 2, complexImg);
    cv::dft(complexImg, complexImg);
    return complexImg;
}

cv::Mat computeIDFT2D(const cv::Mat& complexImg) {
    cv::Mat inverseTransform;
    cv::idft(complexImg, inverseTransform, cv::DFT_SCALE | cv::DFT_REAL_OUTPUT);
    cv::Mat dst;
    inverseTransform.convertTo(dst, CV_8U);
    return dst;
}

double calculateZMNCC(const cv::Mat& img1, const cv::Mat& img2) {
    cv::Mat f1, f2;
    img1.convertTo(f1, CV_64F);
    img2.convertTo(f2, CV_64F);

    cv::Scalar mean1 = cv::mean(f1);
    cv::Scalar mean2 = cv::mean(f2);

    f1 -= mean1[0];
    f2 -= mean2[0];

    double num = f1.dot(f2);
    double den = std::sqrt(f1.dot(f1) * f2.dot(f2));

    return (den != 0) ? (num / den) : 0.0;
}

int main() {
    cv::Mat originalImg = cv::Mat::zeros(256, 256, CV_8UC1);
    cv::rectangle(originalImg, cv::Point(64, 64), cv::Point(192, 192), cv::Scalar(255), -1);

    cv::Mat complexImg = computeDFT2D(originalImg);
    cv::Mat reconstructedImg = computeIDFT2D(complexImg);

    double zmnccVal = calculateZMNCC(originalImg, reconstructedImg);
    std::cout << "[Gorev 9] Orijinal ve Ters DFT Görüntüsü ZMNCC Skoru: " << zmnccVal << std::endl;

    std::cout << "\n================ METRİK RAPORU (GÖREV 9) ================" << std::endl;
    std::cout << "1. ZMNCC (Zero Mean Normalized Cross Correlation): Ortalamayı çıkardığı için ışık/parlaklık değişimlerinden etkilenmez. Korulasyon skoru [-1, 1] arasındadır." << std::endl;
    std::cout << "2. MSE (Mean Squared Error): Pikseller arasındaki ortalama karesel farkı ölçer. Avantajı hızlı ve basit olmasıdır; dezavantajı parlaklık kaymalarına aşırı duyarlı olmasıdır." << std::endl;
    std::cout << "3. SSIM (Structural Similarity Index): İnsan gözünün görsel algısına en uygun metriktir. Yapısal, kontrast ve parlaklık değişimlerini ayrı ayrı kıyaslar." << std::endl;
    std::cout << "==========================================================" << std::endl;

    return 0;
}