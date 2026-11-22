#include <opencv2/opencv.hpp>
#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <vector>

namespace fs = std::filesystem;

const double KIRMIZI_HALKA_ORANI = 0.35;
const double HALKA_KALINLIK_ORANI = 0.25;
const double MIN_ACISAL_KAPSAMA = 0.80;
const double MAX_DIS_KIRMIZI = 0.30;
const int ISLEM_BOYUTU = 400;

struct Cember {
    int x, y, r;
    double oran;
};

cv::Mat kirmiziMaske(const cv::Mat& bgr) {
    cv::Mat hsv, lab, ycrcb;
    cv::cvtColor(bgr, hsv, cv::COLOR_BGR2HSV);
    cv::cvtColor(bgr, lab, cv::COLOR_BGR2Lab);
    cv::cvtColor(bgr, ycrcb, cv::COLOR_BGR2YCrCb);

    cv::Mat mHsv1, mHsv2, mHsv, mLab, mYcc;
    cv::inRange(hsv, cv::Scalar(0, 70, 50), cv::Scalar(10, 255, 255), mHsv1);
    cv::inRange(hsv, cv::Scalar(160, 70, 50), cv::Scalar(180, 255, 255), mHsv2);
    cv::bitwise_or(mHsv1, mHsv2, mHsv);

    std::vector<cv::Mat> labKanal, yccKanal;
    cv::split(lab, labKanal);
    cv::split(ycrcb, yccKanal);
    cv::inRange(labKanal[1], 150, 255, mLab);
    cv::inRange(yccKanal[1], 155, 255, mYcc);

    cv::Mat oy = (mHsv / 255) + (mLab / 255) + (mYcc / 255);
    cv::Mat maske = (oy >= 2);

    cv::Mat cekirdek = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::morphologyEx(maske, maske, cv::MORPH_OPEN, cekirdek);
    cv::morphologyEx(maske, maske, cv::MORPH_CLOSE, cekirdek, cv::Point(-1, -1), 2);
    return maske;
}

std::vector<cv::Vec3i> houghAdaylari(const cv::Mat& gri, const cv::Mat& kirmizi, int minR, int maxR) {
    std::vector<cv::Vec3i> adaylar;
    cv::Mat griBulanik, kirmiziBulanik;
    cv::medianBlur(gri, griBulanik, 5);
    cv::GaussianBlur(kirmizi, kirmiziBulanik, cv::Size(9, 9), 2);

    struct Girdi { cv::Mat goruntu; double p1, p2; };
    std::vector<Girdi> girdiler = {{griBulanik, 100, 30}, {kirmiziBulanik, 100, 20}};

    for (const auto& g : girdiler) {
        std::vector<cv::Vec3f> c;
        cv::HoughCircles(g.goruntu, c, cv::HOUGH_GRADIENT, 1.2,
                         std::max(10, minR * 2), g.p1, g.p2, minR, maxR);
        for (const auto& v : c)
            adaylar.emplace_back(cvRound(v[0]), cvRound(v[1]), cvRound(v[2]));
    }
    return adaylar;
}

double halkaKirmiziOrani(const cv::Mat& kirmizi, int x, int y, int r) {
    cv::Mat halka = cv::Mat::zeros(kirmizi.size(), CV_8UC1);
    int kalinlik = std::max(2, static_cast<int>(r * HALKA_KALINLIK_ORANI));
    cv::circle(halka, cv::Point(x, y), r, 255, kalinlik);
    int toplam = cv::countNonZero(halka);
    if (toplam == 0) return 0.0;
    cv::Mat kesisim;
    cv::bitwise_and(halka, kirmizi, kesisim);
    return static_cast<double>(cv::countNonZero(kesisim)) / toplam;
}

double acisalKapsama(const cv::Mat& kirmizi, int x, int y, int r, int dilim = 36) {
    int dolu = 0;
    for (int i = 0; i < dilim; ++i) {
        double aci = 2.0 * CV_PI * i / dilim;
        for (double oran : {0.8, 0.9, 1.0}) {
            int px = static_cast<int>(x + r * oran * std::cos(aci));
            int py = static_cast<int>(y + r * oran * std::sin(aci));
            if (px >= 0 && px < kirmizi.cols && py >= 0 && py < kirmizi.rows &&
                kirmizi.at<uchar>(py, px)) {
                ++dolu;
                break;
            }
        }
    }
    return static_cast<double>(dolu) / dilim;
}

double disHalkaOrani(const cv::Mat& kirmizi, int x, int y, int r) {
    cv::Mat halka = cv::Mat::zeros(kirmizi.size(), CV_8UC1);
    cv::circle(halka, cv::Point(x, y), static_cast<int>(r * 1.25), 255, -1);
    cv::circle(halka, cv::Point(x, y), static_cast<int>(r * 1.1), 0, -1);
    int toplam = cv::countNonZero(halka);
    if (toplam == 0) return 0.0;
    cv::Mat kesisim;
    cv::bitwise_and(halka, kirmizi, kesisim);
    return static_cast<double>(cv::countNonZero(kesisim)) / toplam;
}

std::vector<Cember> cakisanlariEle(std::vector<Cember> cemberler) {
    std::sort(cemberler.begin(), cemberler.end(),
              [](const Cember& a, const Cember& b) { return a.r > b.r; });
    std::vector<Cember> secilen;
    for (const auto& c : cemberler) {
        bool ayri = true;
        for (const auto& s : secilen) {
            if (std::hypot(c.x - s.x, c.y - s.y) + c.r <= s.r * 1.1) {
                ayri = false;
                break;
            }
        }
        if (ayri) secilen.push_back(c);
    }
    return secilen;
}

std::vector<Cember> tespitEt(const cv::Mat& bgr, cv::Mat& kirmiziBuyuk) {
    double olcek = static_cast<double>(ISLEM_BOYUTU) / std::max(bgr.rows, bgr.cols);
    cv::Mat kucuk;
    cv::resize(bgr, kucuk, cv::Size(), olcek, olcek);

    cv::Mat gri;
    cv::cvtColor(kucuk, gri, cv::COLOR_BGR2GRAY);
    cv::Mat kirmizi = kirmiziMaske(kucuk);

    int kisa = std::min(kucuk.rows, kucuk.cols);
    int minR = std::max(8, static_cast<int>(kisa * 0.05));
    int maxR = static_cast<int>(kisa * 0.55);

    std::vector<Cember> sonuc;
    for (const auto& a : houghAdaylari(gri, kirmizi, minR, maxR)) {
        int x = a[0], y = a[1], r = a[2];
        double oran = halkaKirmiziOrani(kirmizi, x, y, r);
        if (oran >= KIRMIZI_HALKA_ORANI &&
            acisalKapsama(kirmizi, x, y, r) >= MIN_ACISAL_KAPSAMA &&
            disHalkaOrani(kirmizi, x, y, r) <= MAX_DIS_KIRMIZI) {
            sonuc.push_back({x, y, r, oran});
        }
    }
    sonuc = cakisanlariEle(sonuc);

    for (auto& c : sonuc) {
        c.x = static_cast<int>(c.x / olcek);
        c.y = static_cast<int>(c.y / olcek);
        c.r = static_cast<int>(c.r / olcek);
    }
    cv::resize(kirmizi, kirmiziBuyuk, bgr.size(), 0, 0, cv::INTER_NEAREST);
    return sonuc;
}

cv::Mat ciz(const cv::Mat& bgr, const std::vector<Cember>& cemberler) {
    cv::Mat cikti = bgr.clone();
    int kalin = std::max(2, std::max(bgr.rows, bgr.cols) / 200);
    for (const auto& c : cemberler) {
        cv::circle(cikti, cv::Point(c.x, c.y), c.r, cv::Scalar(0, 255, 0), kalin);
        cv::circle(cikti, cv::Point(c.x, c.y), 2, cv::Scalar(255, 0, 0), kalin);
        char yazi[64];
        std::snprintf(yazi, sizeof(yazi), "kirmizi cember %.2f", c.oran);
        cv::putText(cikti, yazi, cv::Point(std::max(0, c.x - c.r), std::max(15, c.y - c.r - 5)),
                    cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 2);
    }
    bool var = !cemberler.empty();
    cv::putText(cikti, var ? "KIRMIZI CEMBERLI ISARET" : "kirmizi cember yok", cv::Point(10, 25),
                cv::FONT_HERSHEY_SIMPLEX, 0.7, var ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255), 2);
    return cikti;
}

std::vector<fs::path> goruntuleriBul(const fs::path& yol) {
    static const std::set<std::string> uzantilar = {".jpg", ".jpeg", ".png", ".bmp", ".ppm", ".webp"};
    std::vector<fs::path> dosyalar;
    if (fs::is_regular_file(yol)) {
        dosyalar.push_back(yol);
        return dosyalar;
    }
    if (!fs::is_directory(yol)) return dosyalar;
    for (const auto& e : fs::recursive_directory_iterator(yol)) {
        if (!e.is_regular_file()) continue;
        std::string uz = e.path().extension().string();
        std::transform(uz.begin(), uz.end(), uz.begin(), ::tolower);
        if (uzantilar.count(uz)) dosyalar.push_back(e.path());
    }
    std::sort(dosyalar.begin(), dosyalar.end());
    return dosyalar;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Kullanim: " << argv[0] << " <goruntu|klasor> [cikti_klasoru] [--goster]" << std::endl;
        return -1;
    }

    fs::path girdi = argv[1];
    std::string ciktiKlasor;
    bool goster = false;
    for (int i = 2; i < argc; ++i) {
        std::string a = argv[i];
        if (a == "--goster") goster = true;
        else ciktiKlasor = a;
    }

    auto dosyalar = goruntuleriBul(girdi);
    if (dosyalar.empty()) {
        std::cerr << "Goruntu bulunamadi: " << girdi << std::endl;
        return -1;
    }
    if (!ciktiKlasor.empty()) fs::create_directories(ciktiKlasor);

    std::ofstream csv;
    if (!ciktiKlasor.empty()) {
        csv.open(fs::path(ciktiKlasor) / "ozet.csv");
        csv << "dosya,cember_sayisi,kirmizi_cemberli_mi\n";
    }

    int toplam = 0, pozitif = 0;
    for (size_t i = 0; i < dosyalar.size(); ++i) {
        cv::Mat bgr = cv::imread(dosyalar[i].string());
        if (bgr.empty()) continue;

        cv::Mat maske;
        auto cemberler = tespitEt(bgr, maske);
        ++toplam;
        if (!cemberler.empty()) ++pozitif;
        std::cout << "[" << i + 1 << "/" << dosyalar.size() << "] " << dosyalar[i].filename().string()
                  << ": " << cemberler.size() << " kirmizi cember" << std::endl;

        cv::Mat cikti = ciz(bgr, cemberler);
        if (!ciktiKlasor.empty()) {
            std::string ad = dosyalar[i].parent_path().filename().string() + "_" +
                             dosyalar[i].stem().string() + ".jpg";
            cv::imwrite((fs::path(ciktiKlasor) / ad).string(), cikti);
            csv << dosyalar[i].string() << "," << cemberler.size() << "," << (cemberler.empty() ? 0 : 1) << "\n";
        }
        if (goster) {
            cv::imshow("Sonuc", cikti);
            cv::imshow("Kirmizi Maske", maske);
            int tus = cv::waitKey(0) & 0xFF;
            if (tus == 'q' || tus == 27) break;
        }
    }

    std::cout << "\nToplam " << toplam << " goruntu, " << pozitif
              << " tanesinde kirmizi cember bulundu." << std::endl;
    cv::destroyAllWindows();
    return 0;
}
