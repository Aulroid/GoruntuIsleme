#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <opencv2/opencv.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void addNoise(std::vector<double>& signal, double noiseRatio) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    for (double& val : signal) {
        double noise = dis(gen) * noiseRatio * std::abs(val != 0 ? val : 1.0);
        val += noise;
    }
}

void plotSignal(const std::string& windowName, const std::vector<double>& signal) {
    int width = 800, height = 400;
    cv::Mat canvas(height, width, CV_8UC3, cv::Scalar(255, 255, 255));
    double minVal = *std::min_element(signal.begin(), signal.end());
    double maxVal = *std::max_element(signal.begin(), signal.end());
    size_t N = signal.size();

    for (size_t i = 0; i < N - 1; ++i) {
        int x1 = static_cast<int>((static_cast<double>(i) / N) * width);
        int y1 = static_cast<int>(height - ((signal[i] - minVal) / (maxVal - minVal + 1e-6)) * (height - 60) - 30);
        int x2 = static_cast<int>((static_cast<double>(i + 1) / N) * width);
        int y2 = static_cast<int>(height - ((signal[i + 1] - minVal) / (maxVal - minVal + 1e-6)) * (height - 60) - 30);
        cv::line(canvas, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 0, 255), 1);
    }
    cv::imshow(windowName, canvas);
    cv::waitKey(0);
}

int main() {
    size_t N = 256;
    std::vector<double> signal(N);
    for (size_t i = 0; i < N; ++i) {
        signal[i] = 10.0 * std::sin(2.0 * M_PI * 5.0 * (static_cast<double>(i) / 256.0));
    }

    double noiseRatio = 0.35; // [0, 1] arasi gurultu orani
    addNoise(signal, noiseRatio);

    std::cout << "[Gorev 2] Gurultulu sinyal gorsellestiriliyor..." << std::endl;
    plotSignal("Gorev 2 - Gurultulu Sinyal", signal);
    return 0;
}