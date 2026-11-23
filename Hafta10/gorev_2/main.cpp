#include "../ortak/stereo_ortak.hpp"

cv::Mat dokuMaskesi(const cv::Mat& gri, int r, float esik) {
    cv::Mat gx, mutlak, toplam;
    cv::Sobel(gri, gx, CV_32F, 1, 0, 3);
    mutlak = cv::abs(gx);
    cv::boxFilter(mutlak, toplam, CV_32F, cv::Size(2 * r + 1, 2 * r + 1));
    return toplam > esik;
}

int main(int argc, char** argv) {
    std::string veriKlasor = st::argVeya(argc, argv, 1, "veri");
    std::string ciktiKlasor = st::argVeya(argc, argv, 2, "sonuclar");
    std::filesystem::create_directories(ciktiKlasor);

    st::Veri v = st::yukle(veriKlasor);

    st::BlokAyar ham;
    ham.r = 4;
    ham.benzersizlik = 0.0f;

    st::BlokAyar filtreli = ham;
    filtreli.benzersizlik = 0.10f;

    st::BlokAyar buyuk = filtreli;
    buyuk.r = 7;

    cv::Mat doku = dokuMaskesi(v.sol, ham.r, 8.0f);

    cv::Mat d1, d2, d3;
    double t1 = st::sureOlc([&] { d1 = st::blokEslestir(v.sol, v.sag, st::sad, ham); });
    double t2 = st::sureOlc([&] { d2 = st::blokEslestir(v.sol, v.sag, st::sad, filtreli, doku); });
    double t3 = st::sureOlc([&] { d3 = st::blokEslestir(v.sol, v.sag, st::sad, buyuk, dokuMaskesi(v.sol, 7, 8.0f)); });

    cv::Ptr<cv::StereoBM> bm = cv::StereoBM::create(st::MAX_DISP, 15);
    bm->setUniquenessRatio(15);
    cv::Mat bm16;
    double tbm = st::sureOlc([&] { bm->compute(v.sol, v.sag, bm16); });
    cv::Mat bmDisp = st::opencvDisp(bm16);

    st::baslikYaz();
    st::satirYaz("Kendi BM 9x9 (ham WTA)", t1, st::degerlendir(d1, v.gt));
    st::satirYaz("Kendi BM 9x9 (+benzersiz)", t2, st::degerlendir(d2, v.gt));
    st::satirYaz("Kendi BM 15x15 (+benzersiz)", t3, st::degerlendir(d3, v.gt));
    st::satirYaz("OpenCV StereoBM 15x15", tbm, st::degerlendir(bmDisp, v.gt));

    cv::imwrite(ciktiKlasor + "/g2_kendi_bm_ham.png", st::renklendir(d1));
    cv::imwrite(ciktiKlasor + "/g2_kendi_bm_filtreli.png", st::renklendir(d2));
    cv::imwrite(ciktiKlasor + "/g2_kendi_bm_15.png", st::renklendir(d3));

    cv::Mat panel;
    cv::hconcat(std::vector<cv::Mat>{st::renklendir(d1), st::renklendir(d2), st::renklendir(bmDisp)}, panel);
    cv::putText(panel, "Kendi BM ham", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(panel, "Kendi BM filtreli", cv::Point(394, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(panel, "OpenCV StereoBM", cv::Point(778, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::imwrite(ciktiKlasor + "/g2_panel.png", panel);

    if (argc > 3 && std::string(argv[3]) == "--goster") {
        cv::imshow("Gorev 2", panel);
        cv::waitKey(0);
    }
    return 0;
}
