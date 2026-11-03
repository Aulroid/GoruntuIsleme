#include <iostream>
#include <vector>
#include <cmath>
#include <complex>
#include <algorithm>
#include <opencv2/opencv.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<std::complex<double>> computeDFT1D(const std::vector<double>& signal) {
    size_t N = signal.size();
    std::vector<std::complex<double>> harmonics(N);
    for (size_t k = 0; k < N; ++k) {
        std::complex<double> sum(0.0, 0.0);
        for (size_t n = 0; n < N; ++n) {
            double angle = -2.0 * M_PI * k * n / N;
            sum += std::complex<double>(signal[n] * std::cos(angle), signal[n] * std::sin(angle));
        }
        harmonics[k] = sum;
    }
    return harmonics;
}

void plotSpectrum(const std::string& title, const std::vector<std::complex<double>>& harmonics) {
    int width = 800, height = 300;
    cv::Mat canvas(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    size_t N = harmonics.size() / 2; // Simetrik olduğu için yarısı gösterilir

    std::vector<double> mags(N);
    for (size_t i = 0; i < N; ++i) mags[i] = std::abs(harmonics[i]);
    double maxVal = *std::max_element(mags.begin(), mags.end());

    for (size_t i = 0; i < N; ++i) {
        int x = static_cast<int>((static_cast<double>(i) / N) * width);
        int y = static_cast<int>(height - (mags[i] / (maxVal + 1e-6)) * (height - 40) - 20);
        cv::line(canvas, cv::Point(x, height - 20), cv::Point(x, y), cv::Scalar(255, 0, 0), 2);
    }
    cv::imshow(title, canvas);
}

int main() {
    size_t N = 256;
    double sampleRate = 256.0;
    std::vector<double> sig1(N), sig2(N), combined(N);

    for (size_t i = 0; i < N; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        double noise1 = ((rand() % 100) / 100.0 - 0.5) * 3.0;
        double noise2 = ((rand() % 100) / 100.0 - 0.5) * 2.0;

        sig1[i] = 12.0 * std::sin(2.0 * M_PI * 50.0 * t) + noise1;
        sig2[i] = 5.0 * std::sin(2.0 * M_PI * 30.0 * t) + noise2;
        combined[i] = sig1[i] + sig2[i];
    }

    auto harmonicsCombined = computeDFT1D(combined);
    plotSpectrum("Gorev 5 - Birlesik Sinyal DFT Harmonik Spektrumu", harmonicsCombined);

    cv::waitKey(0);
    return 0;
}