#include "../ortak/stereo_ortak.hpp"

cv::Mat kenarBul(const cv::Mat& gri, bool sadeceDikey) {
    cv::Mat bulanik, kenar;
    cv::GaussianBlur(gri, bulanik, cv::Size(3, 3), 0);
    cv::Canny(bulanik, kenar, 50, 150);
    if (!sadeceDikey) return kenar;
    cv::Mat gx, gy;
    cv::Sobel(bulanik, gx, CV_32F, 1, 0, 3);
    cv::Sobel(bulanik, gy, CV_32F, 0, 1, 3);
    cv::Mat dikey = cv::abs(gx) > 0.5 * cv::abs(gy);
    cv::Mat sonuc;
    cv::bitwise_and(kenar, dikey, sonuc);
    return sonuc;
}

cv::Mat seyrekGorsel(const cv::Mat& disp, const cv::Mat& arkaPlan) {
    cv::Mat gri, sonuc;
    cv::cvtColor(arkaPlan, gri, cv::COLOR_BGR2GRAY);
    cv::cvtColor(gri, sonuc, cv::COLOR_GRAY2BGR);
    sonuc *= 0.35;
    cv::Mat renkli = st::renklendir(disp);
    cv::Mat buyuk;
    cv::dilate(disp > 0, buyuk, cv::Mat());
    cv::Mat genis;
    cv::dilate(renkli, genis, cv::Mat());
    genis.copyTo(sonuc, buyuk);
    return sonuc;
}

int main(int argc, char** argv) {
    std::string veriKlasor = st::argVeya(argc, argv, 1, "veri");
    std::string ciktiKlasor = st::argVeya(argc, argv, 2, "sonuclar");
    std::filesystem::create_directories(ciktiKlasor);

    st::Veri v = st::yukle(veriKlasor);
    const int tekrar = 5;

    cv::Mat kenarTum, kenarDikey;
    double tKenar = st::sureOlc([&] { kenarTum = kenarBul(v.sol, false); }, tekrar);
    double tKenarDikey = st::sureOlc([&] { kenarDikey = kenarBul(v.sol, true); }, tekrar);

    st::BlokAyar ayar;
    ayar.r = 4;
    ayar.benzersizlik = 0.10f;

    cv::Mat seyrekTum, seyrekDikey;
    double tSeyrekTum = tKenar + st::sureOlc([&] {
        seyrekTum = st::blokEslestir(v.sol, v.sag, st::sad, ayar, kenarTum);
    }, tekrar);
    double tSeyrekDikey = tKenarDikey + st::sureOlc([&] {
        seyrekDikey = st::blokEslestir(v.sol, v.sag, st::sad, ayar, kenarDikey);
    }, tekrar);

    cv::Mat yogunBm;
    double tYogunBm = st::sureOlc([&] { yogunBm = st::blokEslestir(v.sol, v.sag, st::sad, ayar); }, tekrar);

    cv::Mat yogunSgm;
    st::SgmAyar sa;
    double tYogunSgm = st::sureOlc([&] { yogunSgm = st::sgm(v.sol, v.sag, sa); }, 1);

    cv::Ptr<cv::StereoBM> bm = cv::StereoBM::create(st::MAX_DISP, 15);
    bm->setUniquenessRatio(15);
    cv::Mat bm16;
    double tCvBm = st::sureOlc([&] { bm->compute(v.sol, v.sag, bm16); }, tekrar);
    cv::Mat cvBm = st::opencvDisp(bm16);

    const int blok = 5;
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, st::MAX_DISP, blok);
    sgbm->setP1(8 * blok * blok);
    sgbm->setP2(32 * blok * blok);
    sgbm->setDisp12MaxDiff(1);
    sgbm->setUniquenessRatio(10);
    cv::Mat s16;
    double tCvSgbm = st::sureOlc([&] { sgbm->compute(v.sol, v.sag, s16); }, tekrar);
    cv::Mat cvSgbm = st::opencvDisp(s16);

    double toplamPiksel = double(v.sol.rows) * v.sol.cols;
    std::cout << "Kenar pikseli (tum Canny): " << cv::countNonZero(kenarTum) << "  (%" << std::fixed
              << std::setprecision(2) << 100.0 * cv::countNonZero(kenarTum) / toplamPiksel << ")" << std::endl;
    std::cout << "Kenar pikseli (dikey bilesenli): " << cv::countNonZero(kenarDikey) << "  (%"
              << 100.0 * cv::countNonZero(kenarDikey) / toplamPiksel << ")" << std::endl;

    std::cout << "\n[A] Tum goruntu uzerinde (yogunluk = kapsama)" << std::endl;
    st::baslikYaz();
    st::satirYaz("Seyrek: Canny + SAD 9x9", tSeyrekTum, st::degerlendir(seyrekTum, v.gt));
    st::satirYaz("Seyrek: dikey kenar + SAD 9x9", tSeyrekDikey, st::degerlendir(seyrekDikey, v.gt));
    st::satirYaz("Yogun: kendi BM SAD 9x9", tYogunBm, st::degerlendir(yogunBm, v.gt));
    st::satirYaz("Yogun: kendi SGM", tYogunSgm, st::degerlendir(yogunSgm, v.gt));
    st::satirYaz("Yogun: OpenCV StereoBM", tCvBm, st::degerlendir(cvBm, v.gt));
    st::satirYaz("Yogun: OpenCV StereoSGBM", tCvSgbm, st::degerlendir(cvSgbm, v.gt));

    std::cout << "\n[B] Sadece kenar pikselleri uzerinde (adil dogruluk kiyasi)" << std::endl;
    st::baslikYaz();
    st::satirYaz("Seyrek: Canny + SAD 9x9", tSeyrekTum, st::degerlendir(seyrekTum, v.gt, kenarTum));
    st::satirYaz("Yogun: kendi BM SAD 9x9", tYogunBm, st::degerlendir(yogunBm, v.gt, kenarTum));
    st::satirYaz("Yogun: kendi SGM", tYogunSgm, st::degerlendir(yogunSgm, v.gt, kenarTum));
    st::satirYaz("Yogun: OpenCV StereoBM", tCvBm, st::degerlendir(cvBm, v.gt, kenarTum));
    st::satirYaz("Yogun: OpenCV StereoSGBM", tCvSgbm, st::degerlendir(cvSgbm, v.gt, kenarTum));

    cv::imwrite(ciktiKlasor + "/ev_kenarlar.png", kenarTum);
    cv::imwrite(ciktiKlasor + "/ev_seyrek_canny.png", seyrekGorsel(seyrekTum, v.solRenkli));
    cv::imwrite(ciktiKlasor + "/ev_seyrek_dikey.png", seyrekGorsel(seyrekDikey, v.solRenkli));

    cv::Mat kenarRenkli;
    cv::cvtColor(kenarTum, kenarRenkli, cv::COLOR_GRAY2BGR);
    cv::Mat ust, alt, panel;
    cv::hconcat(kenarRenkli, seyrekGorsel(seyrekTum, v.solRenkli), ust);
    cv::hconcat(st::renklendir(yogunBm), st::renklendir(yogunSgm), alt);
    cv::vconcat(ust, alt, panel);
    cv::putText(panel, "Canny kenarlar", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 255, 255), 2);
    cv::putText(panel, "Seyrek fark", cv::Point(394, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(panel, "Yogun BM", cv::Point(10, 308), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(panel, "Yogun SGM", cv::Point(394, 308), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::imwrite(ciktiKlasor + "/ev_panel.png", panel);

    if (argc > 3 && std::string(argv[3]) == "--goster") {
        cv::imshow("Ev Odevi", panel);
        cv::waitKey(0);
    }
    return 0;
}
