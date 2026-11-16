cat << 'EOF' > hafta9/gorev_3/main.cpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

int main() {
    const int width = 7680;
    const int height = 4320;
    std::string inputPath = "8k_orijinal.bmp";
    std::string outputPath = "8k_duzenlenmis.bmp";

    // 1. 8K çözünürlükte 3-kanallı BMP resim oluştur ve kaydet
    std::cout << "8K (7680x4320) 3-kanalli goruntu olusturuluyor..." << std::endl;
    cv::Mat createdImg(height, width, CV_8UC3, cv::Scalar(200, 150, 100));
    cv::imwrite(inputPath, createdImg);

    // 2. OpenCV kullanarak aç
    cv::Mat img = cv::imread(inputPath, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cerr << "8K goruntu acilamadi!" << std::endl;
        return -1;
    }

    // 3. Düzenleme süresi ölçümü BAŞLANGIÇ (Açma ve kaydetme hariç)
    auto start = std::chrono::high_resolution_clock::now();

    // Her bir piksel değerini 2'ye böl (Bellek üzerinde doğrudan pointer erişimi ile)
    for (int r = 0; r < img.rows; ++r) {
        uchar* rowPtr = img.ptr<uchar>(r);
        int totalElements = img.cols * img.channels();
        for (int c = 0; c < totalElements; ++c) {
            rowPtr[c] = rowPtr[c] / 2;
        }
    }

    // Düzenleme süresi ölçümü BİTİŞ
    auto end = std::chrono::high_resolution_clock::now();
    // ---------------------------------------------------------

    std::chrono::duration<double, std::milli> elapsedMs = end - start;
    std::chrono::duration<double, std::micro> elapsedUs = end - start;

    std::cout << "Islem Tamamlandi!\n";
    std::cout << "Sadece Piksel Duzenleme Suresi: " << elapsedMs.count() << " ms (" << elapsedUs.count() << " us)\n";

    // 4. BMP formatında kaydet
    std::cout << "Duzenlenmis 8K goruntu kaydediliyor..." << std::endl;
    cv::imwrite(outputPath, img);
    std::cout << "Kaydedildi: " << outputPath << std::endl;

    return 0;
}
EOF