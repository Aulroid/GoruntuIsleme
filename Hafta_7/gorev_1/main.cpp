#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <opencv2/opencv.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void generateSineWave(std::vector<double>& signal, double phase, double amplitude, double frequency, double sampleRate = 256.0) {
    size_t N = signal.size(); // N değeri 2'nin kuvveti olmalı (örn: 256)
    for (size_t i = 0; i < N; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        signal[i] = amplitude * std::sin(2.0 * M_PI * frequency * t + phase);
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
        cv::line(canvas, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(255, 0, 0), 2);
    }
    cv::imshow(windowName, canvas);
    cv::waitKey(0);
}

int main() {
    size_t N = 256; // 2^8
    std::vector<double> signal(N);

    // Faz: 0, Genlik: 10, Frekans: 5 Hz
    generateSineWave(signal, 0.0, 10.0, 5.0);

    std::cout << "[Gorev 1] Sinus dalgasi olusturuldu ve gorsellestiriliyor..." << std::endl;
    plotSignal("Gorev 1 - Sinuzoidal Dalga", signal);
    return 0;
}