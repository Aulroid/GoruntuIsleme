#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat computeDFT2D(const cv::Mat& inputImg) {
    cv::Mat gray;
    if (inputImg.channels() == 3)
        cv::cvtColor(inputImg, gray, cv::COLOR_BGR2GRAY);
    else
        gray = inputImg.clone();

    cv::Mat padded;
    int m = cv::getOptimalDFTSize(gray.rows);
    int n = cv::getOptimalDFTSize(gray.cols);
    cv::copyMakeBorder(gray, padded, 0, m - gray.rows, 0, n - gray.cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));

    cv::Mat planes[] = { cv::Mat_<float>(padded), cv::Mat::zeros(padded.size(), CV_32F) };
    cv::Mat complexImg;
    cv::merge(planes, 2, complexImg);

    cv::dft(complexImg, complexImg);
    return complexImg; // 2D Harmonik matrisi
}

cv::Mat getMagnitudeSpectrum(const cv::Mat& complexImg) {
    cv::Mat planes[2];
    cv::split(complexImg, planes);
    cv::magnitude(planes[0], planes[1], planes[0]);
    cv::Mat magMat = planes[0];

    magMat += cv::Scalar::all(1);
    cv::log(magMat, magMat);

    magMat = magMat(cv::Rect(0, 0, magMat.cols & -2, magMat.rows & -2));
    int cx = magMat.cols / 2;
    int cy = magMat.rows / 2;

    cv::Mat q0(magMat, cv::Rect(0, 0, cx, cy));
    cv::Mat q1(magMat, cv::Rect(cx, 0, cx, cy));
    cv::Mat q2(magMat, cv::Rect(0, cy, cx, cy));
    cv::Mat q3(magMat, cv::Rect(cx, cy, cx, cy));

    cv::Mat tmp;
    q0.copyTo(tmp); q3.copyTo(q0); tmp.copyTo(q3);
    q1.copyTo(tmp); q2.copyTo(q1); tmp.copyTo(q2);

    cv::normalize(magMat, magMat, 0, 1, cv::NORM_MINMAX);
    return magMat;
}

int main() {
    // Siyah arka planda beyaz kare içeren test görüntüsü
    cv::Mat testImg = cv::Mat::zeros(256, 256, CV_8UC1);
    cv::rectangle(testImg, cv::Point(64, 64), cv::Point(192, 192), cv::Scalar(255), -1);

    cv::Mat harmonics = computeDFT2D(testImg);
    cv::Mat magSpec = getMagnitudeSpectrum(harmonics);

    cv::imshow("Orijinal Resim", testImg);
    cv::imshow("Gorev 7 - 2D DFT Harmonik Spektrumu", magSpec);
    cv::waitKey(0);
    return 0;
}