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

cv::Mat applyFrequencyFilter2D(const cv::Mat& complexImg, const std::string& filterType, double cutoffLow, double cutoffHigh = 0) {
    cv::Mat planes[2];
    cv::split(complexImg, planes);

    int rows = complexImg.rows;
    int cols = complexImg.cols;
    int cx = cols / 2;
    int cy = rows / 2;

    cv::Mat mask = cv::Mat::zeros(rows, cols, CV_32F);

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            double d = std::sqrt(std::pow(i - cy, 2) + std::pow(j - cx, 2));

            if (filterType == "lowpass") {
                mask.at<float>(i, j) = (d <= cutoffLow) ? 1.0f : 0.0f;
            } else if (filterType == "highpass") {
                mask.at<float>(i, j) = (d > cutoffLow) ? 1.0f : 0.0f;
            } else if (filterType == "bandpass") {
                mask.at<float>(i, j) = (d >= cutoffLow && d <= cutoffHigh) ? 1.0f : 0.0f;
            } else if (filterType == "bandreject") {
                mask.at<float>(i, j) = (d < cutoffLow || d > cutoffHigh) ? 1.0f : 0.0f;
            }
        }
    }

    // Maskeyi DFT sırasına göre hizala (Shift)
    cv::Mat q0(mask, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(mask, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(mask, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(mask, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);

    planes[0] = planes[0].mul(mask);
    planes[1] = planes[1].mul(mask);

    cv::Mat filteredComplex;
    cv::merge(planes, 2, filteredComplex);
    return filteredComplex;
}

int main() {
    cv::Mat originalImg = cv::Mat::zeros(256, 256, CV_8UC1);
    cv::rectangle(originalImg, cv::Point(64, 64), cv::Point(192, 192), cv::Scalar(255), -1);

    cv::Mat dftMat = computeDFT2D(originalImg);

    cv::Mat lowPass = computeIDFT2D(applyFrequencyFilter2D(dftMat, "lowpass", 30.0));
    cv::Mat highPass = computeIDFT2D(applyFrequencyFilter2D(dftMat, "highpass", 20.0));
    cv::Mat bandPass = computeIDFT2D(applyFrequencyFilter2D(dftMat, "bandpass", 10.0, 40.0));
    cv::Mat bandReject = computeIDFT2D(applyFrequencyFilter2D(dftMat, "bandreject", 10.0, 40.0));

    cv::imshow("Low-Pass Filtre (Bulanik / Gurultusuz)", lowPass);
    cv::imshow("High-Pass Filtre (Kenarlar Baskin)", highPass);
    cv::imshow("Band-Pass Filtre", bandPass);
    cv::imshow("Band-Reject Filtre", bandReject);

    std::cout << "\n================ YORUM (GÖREV 10) ================" << std::endl;
    std::cout << "- Low-pass: Yuksek frekansli gurultuleri siler fakat kenar gecislerini bulaniklastirir. Genel parlakligi korur." << std::endl;
    std::cout << "- High-pass: Dusuk frekansli yavas degisen parlakliklari siler, resmin ortalamasini 0 yapip sadece kenarlari ve ince detaylari belirginlestirir." << std::endl;
    std::cout << "- Band-pass: Sadece belirli frekans araligindaki detaylarin kalmasini saglar." << std::endl;
    std::cout << "- Band-reject: Belirli frekansa sahip periyodik gurultu bantlarini bastirarak resmi temizler." << std::endl;
    std::cout << "==================================================" << std::endl;

    cv::waitKey(0);
    return 0;
}