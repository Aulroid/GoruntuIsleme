cat << 'EOF' > hafta9/gorev_2/main.cpp
#include <opencv2/opencv.hpp>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

int main() {
    std::string colorPath = "renkli_1000x1000.bmp";
    std::string grayPath = "gri_1000x1000.bmp";

    // Eğer dizinde 1000x1000 renkli bmp yoksa test amaçlı oluştur
    if (!fs::exists(colorPath)) {
        cv::Mat dummy(1000, 1000, CV_8UC3);
        for (int r = 0; r < dummy.rows; ++r) {
            for (int c = 0; c < dummy.cols; ++c) {
                dummy.at<cv::Vec3b>(r, c) = cv::Vec3b(c % 256, r % 256, (r + c) % 256);
            }
        }
        cv::imwrite(colorPath, dummy);
    }

    // 1. Renkli BMP resmini aç ve göster
    cv::Mat colorImg = cv::imread(colorPath, cv::IMREAD_COLOR);
    if (colorImg.empty()) {
        std::cerr << "Resim acilamadi!" << std::endl;
        return -1;
    }

    // Boyut 1000x1000 değilse garantiye almak için yeniden boyutlandır
    if (colorImg.cols != 1000 || colorImg.rows != 1000) {
        cv::resize(colorImg, colorImg, cv::Size(1000, 1000));
        cv::imwrite(colorPath, colorImg);
    }

    cv::imshow("Renkli 1000x1000 BMP", colorImg);

    // 2. Gri tonlu resme dönüştür ve BMP formatında kaydet
    cv::Mat grayImg;
    cv::cvtColor(colorImg, grayImg, cv::COLOR_BGR2GRAY);
    cv::imwrite(grayPath, grayImg);

    cv::imshow("Gri Tonlu 1000x1000 BMP", grayImg);

    // 3. Diskte kapladığı yeri çözünürlük ve kanal sayısı ile kıyasla
    uintmax_t colorFileSize = fs::file_size(colorPath);
    uintmax_t grayFileSize = fs::file_size(grayPath);

    std::cout << "--- DISK BOYUTU VE KANAL KIYASLAMASI ---\n";
    std::cout << "Cozunurluk: " << colorImg.cols << "x" << colorImg.rows << " (" << colorImg.total() << " piksel)\n\n";

    std::cout << "[Renkli Goruntu]\n";
    std::cout << "Kanal Sayisi: " << colorImg.channels() << " (BGR - 24 bit/piksel)\n";
    std::cout << "Teorik Ham Veri Boyutu: " << colorImg.total() * colorImg.channels() << " byte\n";
    std::cout << "Diskteki BMP Boyutu   : " << colorFileSize << " byte (BMP baslik bilgisi dahil)\n\n";

    std::cout << "[Gri Tonlu Goruntu]\n";
    std::cout << "Kanal Sayisi: " << grayImg.channels() << " (Tek kanal - 8 bit/piksel)\n";
    std::cout << "Teorik Ham Veri Boyutu: " << grayImg.total() * grayImg.channels() << " byte\n";
    std::cout << "Diskteki BMP Boyutu   : " << grayFileSize << " byte (BMP baslik + renk paleti dahil)\n\n";

    std::cout << "Sonuc: Sikistirmasiz BMP formatinda dosya boyutu dogrudan (Genislik x Yukseklik x Kanal Sayisi) ile orantilidir.\n";
    std::cout << "Kanal sayisi 3'ten 1'e dustugu icin veri boyutu yaklasik 3 kat azalmistir.\n";

    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}
EOF