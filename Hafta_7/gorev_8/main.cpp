#include <iostream>
#include <random>
#include <opencv2/opencv.hpp>

cv::Mat addGaussianNoise2D(const cv::Mat& src, double mean, double stddev) {
    cv::Mat noise = cv::Mat(src.size(), CV_32FC1);
    cv::randn(noise, mean, stddev);
    cv::Mat dst;
    src.convertTo(dst, CV_32F);
    dst = dst + noise;
    dst.convertTo(dst, CV_8U);
    return dst;
}

cv::Mat addSaltAndPepperNoise2D(const cv::Mat& src, double noiseRatio) {
    cv::Mat dst = src.clone();
    int numNoise = static_cast<int>(src.rows * src.cols * noiseRatio);
    std::default_random_engine generator;
    std::uniform_int_distribution<int> rowDist(0, src.rows - 1);
    std::uniform_int_distribution<int> colDist(0, src.cols - 1);

    for (int i = 0; i < numNoise / 2; ++i) {
        dst.at<uchar>(rowDist(generator), colDist(generator)) = 255; // Salt
        dst.at<uchar>(rowDist(generator), colDist(generator)) = 0;   // Pepper
    }
    return dst;
}

cv::Mat computeDFT2D(const cv::Mat& inputImg) {
    cv::Mat gray = inputImg.clone();
    cv::Mat padded;
    int m = cv::getOptimalDFTSize(gray.rows);
    int n = cv::getOptimalDFTSize(gray.cols);
    cv::copyMakeBorder(gray, padded, 0, m - gray.rows, 0, n - gray.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
    cv::Mat complexImg;
    cv::merge(planes, 2, complexImg);
    cv::dft(complexImg, complexImg);
    return complexImg;
}

cv::Mat getMagnitudeSpectrum(const cv::Mat& complexImg) {
    cv::Mat planes[2];
    cv::split(complexImg, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);
    cv::Mat magMat = planes[0];
    magMat += cv::Scalar::all(1);
    cv::log(magMat, magMat);
    magMat = magMat(cv::Rect(0, 0, magMat.cols & -2, magMat.rows & -2));
    int cx = magMat.cols / 2, cy = magMat.rows / 2;
    cv::Mat q0(magMat, cv::Rect(0, 0, cx, cy)), q1(magMat, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(magMat, cv::Rect(0, cy, cx, cy)), q3(magMat, cv::Rect(cx, cy, cx, cy));
    cv::Mat tmp;
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);
    cv::normalize(magMat, magMat, 0, 1, cv::NORM_MINMAX);
    return magMat;
}

int main() {
    cv::Mat img = cv::Mat::zeros(256, 256, CV_8UC1);
    cv::rectangle(img, cv::Point(64, 64), cv::Point(192, 192), cv::Scalar(255), -1);

    cv::Mat gaussImg = addGaussianNoise2D(img, 0, 30);
    cv::Mat spImg = addSaltAndPepperNoise2D(img, 0.05);

    cv::Mat origSpec = getMagnitudeSpectrum(computeDFT2D(img));
    cv::Mat gaussSpec = getMagnitudeSpectrum(computeDFT2D(gaussImg));
    cv::Mat spSpec = getMagnitudeSpectrum(computeDFT2D(spImg));

    cv::imshow("Orijinal Spektrum", origSpec);
    cv::imshow("Gauss Gurultulu Spektrum", gaussSpec);
    cv::imshow("Salt-Pepper Gurultulu Spektrum", spSpec);

    std::cout << "[KIYASLAMA - GOREV 8]: Gurultu eklendiginde frekans spektrumunda yuksek frekans bolgelerine (merkezden uzak alanlar) rastgele beyaz parazit benekleri/noktalari yayilir." << std::endl;
    cv::waitKey(0);
    return 0;
}