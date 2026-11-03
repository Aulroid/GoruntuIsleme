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

std::vector<double> computeIDFT1D(const std::vector<std::complex<double>>& harmonics) {
    size_t N = harmonics.size();
    std::vector<double> signal(N);
    for (size_t n = 0; n < N; ++n) {
        std::complex<double> sum(0.0, 0.0);
        for (size_t k = 0; k < N; ++k) {
            double angle = 2.0 * M_PI * k * n / N;
            sum += harmonics[k] * std::complex<double>(std::cos(angle), std::sin(angle));
        }
        signal[n] = sum.real() / N;
    }
    return signal;
}

std::vector<double> frequencyFilter1D(const std::vector<std::complex<double>>& harmonics, double threshold) {
    std::vector<std::complex<double>> filteredHarmonics = harmonics;
    for (auto& h : filteredHarmonics) {
        if (std::abs(h) < threshold) {
            h = std::complex<double>(0.0, 0.0);
        }
    }
    return computeIDFT1D(filteredHarmonics);
}

void plotSignal(const std::string& windowName, const std::vector<double>& signal) {
    int width = 800, height = 300;
    cv::Mat canvas(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    double minVal = *std::min_element(signal.begin(), signal.end());
    double maxVal = *std::max_element(signal.begin(), signal.end());
    size_t N = signal.size();

    for (size_t i = 0; i < N - 1; ++i) {
        int x1 = static_cast<int>((static_cast<double>(i) / N) * width);
        int y1 = static_cast<int>(height - ((signal[i] - minVal) / (maxVal - minVal + 1e-6)) * (height - 60) - 30);
        int x2 = static_cast<int>((static_cast<double>(i + 1) / N) * width);
        int y2 = static_cast<int>(height - ((signal[i + 1] - minVal) / (maxVal - minVal + 1e-6)) * (height - 60) - 30);
        cv::line(canvas, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 150, 0), 2);
    }
    cv::imshow(windowName, canvas);
}

int main() {
    size_t N = 256;
    std::vector<double> combined(N);
    for (size_t i = 0; i < N; ++i) {
        double t = static_cast<double>(i) / 256.0;
        combined[i] = 12.0 * std::sin(2.0 * M_PI * 50.0 * t) + 5.0 * std::sin(2.0 * M_PI * 30.0 * t) + ((rand() % 100) / 100.0 - 0.5) * 4.0;
    }

    auto harmonics = computeDFT1D(combined);
    double threshold = 250.0; // Esik alti gürültü harmonikleri sifirlanir
    auto filteredSignal = frequencyFilter1D(harmonics, threshold);

    plotSignal("Gorev 6 - Frekans Filtresi ile Temizlenmis Yeniden Uretilmis Sinyal", filteredSignal);
    cv::waitKey(0);
    return 0;
}