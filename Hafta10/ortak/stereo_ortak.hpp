#pragma once

#include <opencv2/opencv.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <vector>

namespace st {

const int MAX_DISP = 48;
const double GT_OLCEK = 16.0;
const double GT_CARPAN = 2.0;
const double HATA_ESIGI = 2.0;

struct Veri {
    cv::Mat solRenkli, sagRenkli;
    cv::Mat sol, sag;
    cv::Mat gt;
};

inline cv::Mat gtSolaTasi(const cv::Mat& gtSag) {
    cv::Mat gt = cv::Mat::zeros(gtSag.size(), CV_32F);
    for (int y = 0; y < gtSag.rows; ++y) {
        for (int x = 0; x < gtSag.cols; ++x) {
            float d = gtSag.at<uchar>(y, x) / GT_OLCEK * GT_CARPAN;
            if (d <= 0) continue;
            int xl = x + cvRound(d);
            if (xl < 0 || xl >= gt.cols) continue;
            gt.at<float>(y, xl) = std::max(gt.at<float>(y, xl), d);
        }
    }
    return gt;
}

inline Veri yukle(const std::string& klasor) {
    Veri v;
    v.solRenkli = cv::imread(klasor + "/scene1.row3.col1.ppm", cv::IMREAD_COLOR);
    v.sagRenkli = cv::imread(klasor + "/scene1.row3.col3.ppm", cv::IMREAD_COLOR);
    cv::Mat gtSag = cv::imread(klasor + "/truedisp.row3.col3.pgm", cv::IMREAD_GRAYSCALE);
    if (v.solRenkli.empty() || v.sagRenkli.empty() || gtSag.empty()) {
        std::cerr << "Veri okunamadi: " << klasor << std::endl;
        std::exit(-1);
    }
    cv::cvtColor(v.solRenkli, v.sol, cv::COLOR_BGR2GRAY);
    cv::cvtColor(v.sagRenkli, v.sag, cv::COLOR_BGR2GRAY);
    v.gt = gtSolaTasi(gtSag);
    return v;
}

struct Olcum {
    double kotuPiksel;
    double ortalamaHata;
    double kapsama;
    int degerlendirilen;
};

inline Olcum degerlendir(const cv::Mat& disp, const cv::Mat& gt, const cv::Mat& maske = cv::Mat()) {
    int toplam = 0, gecerli = 0, kotu = 0;
    double hataToplam = 0;
    for (int y = 0; y < gt.rows; ++y) {
        for (int x = MAX_DISP; x < gt.cols; ++x) {
            float g = gt.at<float>(y, x);
            if (g <= 0) continue;
            if (!maske.empty() && !maske.at<uchar>(y, x)) continue;
            ++toplam;
            float d = disp.at<float>(y, x);
            if (d <= 0) continue;
            ++gecerli;
            double e = std::abs(d - g);
            hataToplam += e;
            if (e > HATA_ESIGI) ++kotu;
        }
    }
    Olcum o;
    o.degerlendirilen = toplam;
    o.kapsama = toplam ? 100.0 * gecerli / toplam : 0;
    o.kotuPiksel = gecerli ? 100.0 * kotu / gecerli : 0;
    o.ortalamaHata = gecerli ? hataToplam / gecerli : 0;
    return o;
}

inline cv::Mat renklendir(const cv::Mat& disp) {
    cv::Mat d8;
    disp.convertTo(d8, CV_8U, 255.0 / MAX_DISP);
    cv::Mat renkli;
    cv::applyColorMap(d8, renkli, cv::COLORMAP_JET);
    renkli.setTo(cv::Scalar(0, 0, 0), disp <= 0);
    return renkli;
}

inline cv::Mat opencvDisp(const cv::Mat& disp16) {
    cv::Mat d;
    disp16.convertTo(d, CV_32F, 1.0 / 16.0);
    d.setTo(-1, d <= 0);
    return d;
}

template <typename F>
double sureOlc(F f, int tekrar = 1) {
    auto b = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < tekrar; ++i) f();
    auto s = std::chrono::high_resolution_clock::now();
    return std::chrono::duration<double, std::milli>(s - b).count() / tekrar;
}

inline void satirYaz(const std::string& ad, double ms, const Olcum& o) {
    std::cout << std::left << std::setw(38) << ad << std::right << std::fixed << std::setprecision(2)
              << std::setw(10) << ms << " ms" << std::setw(10) << o.kotuPiksel << " %" << std::setw(9)
              << o.ortalamaHata << std::setw(10) << o.kapsama << " %" << std::endl;
}

inline void baslikYaz() {
    std::cout << std::left << std::setw(38) << "Yontem" << std::right << std::setw(13) << "Sure"
              << std::setw(12) << "Kotu(>2px)" << std::setw(9) << "OrtHata" << std::setw(12) << "Kapsama"
              << std::endl;
    std::cout << std::string(84, '-') << std::endl;
}

typedef float (*BenzerlikFonk)(const cv::Mat& sol, const cv::Mat& sag, int x, int y, int d, int r);

inline float sad(const cv::Mat& sol, const cv::Mat& sag, int x, int y, int d, int r) {
    float t = 0;
    for (int dy = -r; dy <= r; ++dy) {
        const uchar* a = sol.ptr<uchar>(y + dy);
        const uchar* b = sag.ptr<uchar>(y + dy);
        for (int dx = -r; dx <= r; ++dx) t += std::abs(a[x + dx] - b[x + dx - d]);
    }
    return t;
}

inline float ssd(const cv::Mat& sol, const cv::Mat& sag, int x, int y, int d, int r) {
    float t = 0;
    for (int dy = -r; dy <= r; ++dy) {
        const uchar* a = sol.ptr<uchar>(y + dy);
        const uchar* b = sag.ptr<uchar>(y + dy);
        for (int dx = -r; dx <= r; ++dx) {
            float f = float(a[x + dx]) - float(b[x + dx - d]);
            t += f * f;
        }
    }
    return t;
}

inline float zncc(const cv::Mat& sol, const cv::Mat& sag, int x, int y, int d, int r) {
    float ta = 0, tb = 0, taa = 0, tbb = 0, tab = 0;
    int n = (2 * r + 1) * (2 * r + 1);
    for (int dy = -r; dy <= r; ++dy) {
        const uchar* a = sol.ptr<uchar>(y + dy);
        const uchar* b = sag.ptr<uchar>(y + dy);
        for (int dx = -r; dx <= r; ++dx) {
            float va = a[x + dx], vb = b[x + dx - d];
            ta += va;
            tb += vb;
            taa += va * va;
            tbb += vb * vb;
            tab += va * vb;
        }
    }
    float kovA = taa - ta * ta / n;
    float kovB = tbb - tb * tb / n;
    if (kovA < 1e-3f || kovB < 1e-3f) return 2.0f;
    float ncc = (tab - ta * tb / n) / std::sqrt(kovA * kovB);
    return 1.0f - ncc;
}

struct BlokAyar {
    int maxDisp = MAX_DISP;
    int r = 4;
    float benzersizlik = 0.0f;
    bool altPiksel = true;
};

inline float pikselEslestir(const cv::Mat& sol, const cv::Mat& sag, int x, int y, BenzerlikFonk fonk,
                            const BlokAyar& a, std::vector<float>& maliyet) {
    int dMax = std::min(a.maxDisp - 1, x - a.r);
    if (dMax < 1) return -1;
    int enIyi = 0;
    for (int d = 0; d <= dMax; ++d) {
        maliyet[d] = fonk(sol, sag, x, y, d, a.r);
        if (maliyet[d] < maliyet[enIyi]) enIyi = d;
    }
    if (a.benzersizlik > 0) {
        float ikinci = std::numeric_limits<float>::max();
        for (int d = 0; d <= dMax; ++d)
            if (std::abs(d - enIyi) > 1) ikinci = std::min(ikinci, maliyet[d]);
        if (ikinci < maliyet[enIyi] * (1.0f + a.benzersizlik) + 1e-6f) return -1;
    }
    float sonuc = float(enIyi);
    if (a.altPiksel && enIyi > 0 && enIyi < dMax) {
        float c0 = maliyet[enIyi - 1], c1 = maliyet[enIyi], c2 = maliyet[enIyi + 1];
        float payda = c0 - 2 * c1 + c2;
        if (payda > 1e-6f) sonuc += 0.5f * (c0 - c2) / payda;
    }
    return sonuc > 0 ? sonuc : -1;
}

inline cv::Mat blokEslestir(const cv::Mat& sol, const cv::Mat& sag, BenzerlikFonk fonk, const BlokAyar& a,
                            const cv::Mat& maske = cv::Mat()) {
    cv::Mat disp(sol.size(), CV_32F, cv::Scalar(-1));
    cv::parallel_for_(cv::Range(a.r, sol.rows - a.r), [&](const cv::Range& aralik) {
        std::vector<float> maliyet(a.maxDisp);
        for (int y = aralik.start; y < aralik.end; ++y) {
            float* satir = disp.ptr<float>(y);
            const uchar* m = maske.empty() ? nullptr : maske.ptr<uchar>(y);
            for (int x = a.r; x < sol.cols - a.r; ++x) {
                if (m && !m[x]) continue;
                satir[x] = pikselEslestir(sol, sag, x, y, fonk, a, maliyet);
            }
        }
    });
    return disp;
}

struct SgmAyar {
    int maxDisp = MAX_DISP;
    int pencere = 5;
    int P1 = 200;
    int P2 = 800;
    int yonSayisi = 8;
    bool solSagKontrol = true;
};

inline cv::Mat sgm(const cv::Mat& sol, const cv::Mat& sag, const SgmAyar& a) {
    const int H = sol.rows, W = sol.cols, D = a.maxDisp;
    const int buyukMaliyet = 255 * a.pencere * a.pencere;
    std::vector<int32_t> C((size_t)H * W * D), L((size_t)H * W * D), S((size_t)H * W * D, 0);
    auto idx = [&](int y, int x, int d) { return ((size_t)y * W + x) * D + d; };

    cv::Mat solF, sagF;
    sol.convertTo(solF, CV_32F);
    sag.convertTo(sagF, CV_32F);
    for (int d = 0; d < D; ++d) {
        cv::Mat kayik(H, W, CV_32F, cv::Scalar(0));
        sagF(cv::Rect(0, 0, W - d, H)).copyTo(kayik(cv::Rect(d, 0, W - d, H)));
        cv::Mat fark, toplam;
        cv::absdiff(solF, kayik, fark);
        cv::boxFilter(fark, toplam, CV_32F, cv::Size(a.pencere, a.pencere), cv::Point(-1, -1), false,
                      cv::BORDER_REPLICATE);
        for (int y = 0; y < H; ++y) {
            const float* t = toplam.ptr<float>(y);
            for (int x = 0; x < W; ++x)
                C[idx(y, x, d)] = (x - d < a.pencere / 2) ? buyukMaliyet : (int32_t)t[x];
        }
    }

    const int yonler[8][2] = {{1, 0}, {-1, 0}, {0, 1}, {0, -1}, {1, 1}, {-1, 1}, {1, -1}, {-1, -1}};
    for (int yi = 0; yi < a.yonSayisi; ++yi) {
        int dx = yonler[yi][0], dy = yonler[yi][1];
        int y0 = dy >= 0 ? 0 : H - 1, y1 = dy >= 0 ? H : -1, sy = dy >= 0 ? 1 : -1;
        int x0 = dx >= 0 ? 0 : W - 1, x1 = dx >= 0 ? W : -1, sx = dx >= 0 ? 1 : -1;
        for (int y = y0; y != y1; y += sy) {
            for (int x = x0; x != x1; x += sx) {
                int py = y - dy, px = x - dx;
                int32_t* l = &L[idx(y, x, 0)];
                const int32_t* c = &C[idx(y, x, 0)];
                if (py < 0 || py >= H || px < 0 || px >= W) {
                    for (int d = 0; d < D; ++d) l[d] = c[d];
                } else {
                    const int32_t* lp = &L[idx(py, px, 0)];
                    int32_t minOnceki = *std::min_element(lp, lp + D);
                    for (int d = 0; d < D; ++d) {
                        int32_t m = lp[d];
                        if (d > 0) m = std::min(m, lp[d - 1] + a.P1);
                        if (d < D - 1) m = std::min(m, lp[d + 1] + a.P1);
                        m = std::min(m, minOnceki + a.P2);
                        l[d] = c[d] + m - minOnceki;
                    }
                }
                int32_t* s = &S[idx(y, x, 0)];
                for (int d = 0; d < D; ++d) s[d] += l[d];
            }
        }
    }

    cv::Mat dispSol(H, W, CV_32F, cv::Scalar(-1));
    cv::Mat dispSag(H, W, CV_32S, cv::Scalar(-1));
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            const int32_t* s = &S[idx(y, x, 0)];
            int enIyi = int(std::min_element(s, s + D) - s);
            float sonuc = float(enIyi);
            if (enIyi > 0 && enIyi < D - 1) {
                float c0 = s[enIyi - 1], c1 = s[enIyi], c2 = s[enIyi + 1];
                float payda = c0 - 2 * c1 + c2;
                if (payda > 0) sonuc += 0.5f * (c0 - c2) / payda;
            }
            dispSol.at<float>(y, x) = sonuc;
        }
        for (int xr = 0; xr < W; ++xr) {
            int enIyi = -1;
            int32_t enKucuk = std::numeric_limits<int32_t>::max();
            for (int d = 0; d < D && xr + d < W; ++d) {
                int32_t v = S[idx(y, xr + d, d)];
                if (v < enKucuk) {
                    enKucuk = v;
                    enIyi = d;
                }
            }
            dispSag.at<int>(y, xr) = enIyi;
        }
    }

    cv::Mat sonuc;
    cv::medianBlur(dispSol, sonuc, 3);
    for (int y = 0; y < H; ++y) {
        for (int x = 0; x < W; ++x) {
            float d = sonuc.at<float>(y, x);
            if (d <= 0) {
                sonuc.at<float>(y, x) = -1;
                continue;
            }
            if (!a.solSagKontrol) continue;
            int xr = x - cvRound(d);
            if (xr < 0 || std::abs(dispSag.at<int>(y, xr) - d) > 1.0f) sonuc.at<float>(y, x) = -1;
        }
    }
    return sonuc;
}

inline std::string argVeya(int argc, char** argv, int i, const std::string& vars) {
    return argc > i ? std::string(argv[i]) : vars;
}

}
