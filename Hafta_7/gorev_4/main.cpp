#include <iostream>
#include <vector>
#include <cmath>
#include <complex>

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

int main() {
    size_t N = 256;
    std::vector<double> cleanSignal(N), noisySignal(N);

    for (size_t i = 0; i < N; ++i) {
        cleanSignal[i] = 10.0 * std::sin(2.0 * M_PI * 5.0 * (static_cast<double>(i) / 256.0));
        noisySignal[i] = cleanSignal[i] + ((rand() % 100) / 50.0 - 1.0) * 4.0;
    }

    auto cleanHarmonics = computeDFT1D(cleanSignal);
    auto noisyHarmonics = computeDFT1D(noisySignal);

    std::cout << "[Gorev 4] DFT Islemi Tamamlandi." << std::endl;
    std::cout << "\n[YORUM - GOREV 4]:" << std::endl;
    std::cout << "- Gurultusuz Sinyal Harmonikleri: Sadece ana sinyal frekansina karsilik gelen k katsayilarinda tepe (peak) verir, diger frekanslar 0'a yakindir." << std::endl;
    std::cout << "- Gurultulu Sinyal Harmonikleri: Ana tepe noktalarinin haricinde tum frekans spektrumuna yayilmis dusuk/orta genlikli parazit harmonikler icerir." << std::endl;

    return 0;
}