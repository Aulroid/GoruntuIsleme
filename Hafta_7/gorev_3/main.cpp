#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <opencv2/opencv.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

std::vector<double> apply1DGaussian(const std::vector<double>& signal, int kernelSize = 5, double sigma = 1.0) {
    std::vector<double> result(signal.size());
    std::vector<double> kernel(kernelSize);
    int half = kernelSize / 2;
    double sum = 0.0;

    for (int i = -half; i <= half; ++i) {
        kernel[i + half] = std::exp(-(i * i) / (2 * sigma * sigma));
        sum += kernel[i + half];
    }
    for (double& k : kernel) k /= sum;

    for (size_t i = 0; i < signal.size(); ++i) {
        double val = 0.0;
        for (int j = -half; j <= half; ++j) {
            int idx = std::clamp(static_cast<int>(i) + j, 0, static_cast<int>(signal.size()) - 1);
            val += signal[idx] * kernel[j + half];
        }
        result[i] = val;
    }
    return result;
}

std::vector<double> apply1DMedian(const std::vector<double>& signal, int windowSize = 5) {
    std::vector<double> result(signal.size());
    int half = windowSize / 2;

    for (size_t i = 0; i < signal.size(); ++i) {
        std::vector<double> window;
        for (int j = -half; j <= half; ++j) {
            int idx = std::clamp(static_cast<int>(i) + j, 0, static_cast<int>(signal.size()) - 1);
            window.push_back(signal[idx]);
        }
        std::sort(window.begin(), window.end());
        result[i] = window[window.size() / 2];
    }
    return result;
}

void plotSignal(const std::string& windowName, const std::vector<double>& signal, cv::Scalar color) {
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
        cv::line(canvas, cv::Point(x1, y1), cv::Point(x2, y2), color, 2);
    }
    cv::imshow(windowName, canvas);
}

int main() {
    size_t N = 256;
    std::vector<double> noisySignal(N);
    std::random_device rd; std::mt19937 gen(rd()); std::uniform_real_distribution<> dis(-3.0, 3.0);

    for (size_t i = 0; i < N; ++i) {
        noisySignal[i] = 10.0 * std::sin(2.0 * M_PI * 5.0 * (static_cast<double>(i) / 256.0)) + dis(gen);
    }

    auto gaussFiltered = apply1DGaussian(noisySignal, 5, 1.0);
    auto medianFiltered = apply1DMedian(noisySignal, 5);

    plotSignal("Gurultulu Sinyal", noisySignal, cv::Scalar(0, 0, 255));
    plotSignal("Gauss Filtreli Sinyal", gaussFiltered, cv::Scalar(0, 255, 0));
    plotSignal("Medyan Filtreli Sinyal", medianFiltered, cv::Scalar(255, 0, 0));

    std::cout << "\n[YORUM - GOREV 3]:" << std::endl;
    std::cout << "- Gauss Filtresi: Rastgele surekli gurultuleri yumusatmakta başarılıdır ancak genlik tepelerini hafif basıklaştırır (blur etkisi)." << std::endl;
    std::cout << "- Medyan Filtresi: Ani ve aykiri gurultuleri sinyalin genel dalga yapisini ve genlik zirvelerini bozmadan temizler." << std::endl;

    cv::waitKey(0);
    return 0;
}