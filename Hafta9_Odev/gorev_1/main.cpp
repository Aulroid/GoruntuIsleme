#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>

const int FARK_ESIGI = 25;
const int MIN_HAREKETLI_PIKSEL = 1500;
const double MIN_KONTUR_ALANI = 500.0;
const int ROI_PAYI = 20;

const cv::Scalar YCRCB_ALT(0, 133, 77);
const cv::Scalar YCRCB_UST(255, 173, 127);
const cv::Scalar HSV_ALT(0, 30, 60);
const cv::Scalar HSV_UST(25, 180, 255);
const cv::Vec3b TEN_RENGI(0, 255, 0);

cv::Mat onIsle(const cv::Mat& kare) {
    cv::Mat gri;
    cv::cvtColor(kare, gri, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gri, gri, cv::Size(21, 21), 0);
    return gri;
}

cv::Mat hareketMaskesi(const cv::Mat& oncekiGri, const cv::Mat& simdikiGri) {
    cv::Mat fark, maske;
    cv::absdiff(oncekiGri, simdikiGri, fark);
    cv::threshold(fark, maske, FARK_ESIGI, 255, cv::THRESH_BINARY);
    cv::Mat cekirdek = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(5, 5));
    cv::morphologyEx(maske, maske, cv::MORPH_OPEN, cekirdek, cv::Point(-1, -1), 1);
    cv::dilate(maske, maske, cekirdek, cv::Point(-1, -1), 3);
    return maske;
}

std::vector<cv::Rect> roiKutulari(const cv::Mat& maske) {
    std::vector<std::vector<cv::Point>> konturlar;
    cv::findContours(maske.clone(), konturlar, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<cv::Rect> kutular;
    cv::Rect sinir(0, 0, maske.cols, maske.rows);
    for (const auto& c : konturlar) {
        if (cv::contourArea(c) < MIN_KONTUR_ALANI) continue;
        cv::Rect r = cv::boundingRect(c);
        r.x -= ROI_PAYI;
        r.y -= ROI_PAYI;
        r.width += 2 * ROI_PAYI;
        r.height += 2 * ROI_PAYI;
        r &= sinir;
        if (r.area() > 0) kutular.push_back(r);
    }
    return kutular;
}

cv::Mat tenMaskesi(const cv::Mat& bolge) {
    cv::Mat ycrcb, hsv, m1, m2, maske;
    cv::cvtColor(bolge, ycrcb, cv::COLOR_BGR2YCrCb);
    cv::cvtColor(bolge, hsv, cv::COLOR_BGR2HSV);
    cv::inRange(ycrcb, YCRCB_ALT, YCRCB_UST, m1);
    cv::inRange(hsv, HSV_ALT, HSV_UST, m2);
    cv::bitwise_and(m1, m2, maske);
    cv::Mat cekirdek = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::morphologyEx(maske, maske, cv::MORPH_OPEN, cekirdek, cv::Point(-1, -1), 2);
    cv::morphologyEx(maske, maske, cv::MORPH_CLOSE, cekirdek, cv::Point(-1, -1), 2);
    cv::GaussianBlur(maske, maske, cv::Size(3, 3), 0);
    return maske;
}

bool kareyiIsle(const cv::Mat& kare, cv::Mat& oncekiGri, cv::Mat& cikti, cv::Mat& hMaske, cv::Mat& genelTen) {
    cv::Mat simdikiGri = onIsle(kare);
    hMaske = hareketMaskesi(oncekiGri, simdikiGri);
    int hareketliPiksel = cv::countNonZero(hMaske);
    bool hareketVar = hareketliPiksel >= MIN_HAREKETLI_PIKSEL;

    cv::Mat gri, arkaPlan;
    cv::cvtColor(kare, gri, cv::COLOR_BGR2GRAY);
    cv::cvtColor(gri, arkaPlan, cv::COLOR_GRAY2BGR);
    arkaPlan.convertTo(cikti, -1, 0.3, 0);
    genelTen = cv::Mat::zeros(kare.size(), CV_8UC1);

    if (hareketVar) {
        for (const cv::Rect& r : roiKutulari(hMaske)) {
            cv::Mat roi = kare(r);
            roi.copyTo(cikti(r));
            cv::Mat tMaske = tenMaskesi(roi);
            cv::Mat genelRoi = genelTen(r);
            cv::bitwise_or(genelRoi, tMaske, genelRoi);

            cv::Mat renkli = cikti(r).clone();
            renkli.setTo(TEN_RENGI, tMaske > 0);
            cv::Mat hedef = cikti(r);
            cv::addWeighted(hedef, 0.4, renkli, 0.6, 0, hedef);
            cv::rectangle(cikti, r, cv::Scalar(0, 0, 255), 2);
            cv::putText(cikti, "ROI", cv::Point(r.x, std::max(15, r.y - 5)),
                        cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
        }
    }

    std::string durum = "Hareketli piksel: " + std::to_string(hareketliPiksel) +
                        (hareketVar ? "  HAREKET VAR" : "  hareket yok");
    cv::putText(cikti, durum, cv::Point(10, 25), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                hareketVar ? cv::Scalar(0, 255, 255) : cv::Scalar(200, 200, 200), 2);
    oncekiGri = simdikiGri;
    return hareketVar;
}

int main(int argc, char** argv) {
    cv::VideoCapture kamera;
    bool videoMu = false;
    if (argc > 1) {
        std::string arg = argv[1];
        bool sayiMi = !arg.empty() && arg.find_first_not_of("0123456789") == std::string::npos;
        if (sayiMi) {
            kamera.open(std::stoi(arg));
        } else {
            kamera.open(arg);
            videoMu = true;
        }
    } else {
        kamera.open(0);
    }

    if (!kamera.isOpened()) {
        std::cerr << "Kamera/video acilamadi!" << std::endl;
        return -1;
    }

    cv::Mat kare;
    if (!kamera.read(kare)) {
        std::cerr << "Ilk kare okunamadi!" << std::endl;
        return -1;
    }
    if (!videoMu) cv::flip(kare, kare, 1);
    cv::Mat oncekiGri = onIsle(kare);

    cv::Mat cikti, hMaske, tMaske;
    while (kamera.read(kare)) {
        if (!videoMu) cv::flip(kare, kare, 1);
        kareyiIsle(kare, oncekiGri, cikti, hMaske, tMaske);

        cv::imshow("Orijinal", kare);
        cv::imshow("Hareket Maskesi", hMaske);
        cv::imshow("Ten Maskesi (sadece ROI)", tMaske);
        cv::imshow("Sonuc", cikti);

        int tus = cv::waitKey(1) & 0xFF;
        if (tus == 'q' || tus == 27) break;
        if (tus == 's') {
            std::string ad = "sonuc_" + std::to_string(std::time(nullptr)) + ".png";
            cv::imwrite(ad, cikti);
            std::cout << "Kaydedildi: " << ad << std::endl;
        }
    }

    kamera.release();
    cv::destroyAllWindows();
    return 0;
}
