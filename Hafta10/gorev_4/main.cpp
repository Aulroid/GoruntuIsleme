#include "../ortak/stereo_ortak.hpp"

int main(int argc, char** argv) {
    std::string veriKlasor = st::argVeya(argc, argv, 1, "veri");
    std::string ciktiKlasor = st::argVeya(argc, argv, 2, "sonuclar");
    std::filesystem::create_directories(ciktiKlasor);

    st::Veri v = st::yukle(veriKlasor);

    struct Deneme {
        std::string ad;
        int P1, P2, yon;
    };
    std::vector<Deneme> denemeler = {
        {"P1=0    P2=0     (8 yon)", 0, 0, 8},
        {"P1=50   P2=200   (8 yon)", 50, 200, 8},
        {"P1=200  P2=800   (8 yon)", 200, 800, 8},
        {"P1=800  P2=3200  (8 yon)", 800, 3200, 8},
        {"P1=2000 P2=8000  (8 yon)", 2000, 8000, 8},
        {"P1=200  P2=800   (4 yon)", 200, 800, 4},
    };

    st::baslikYaz();
    std::vector<cv::Mat> gorseller;
    for (size_t i = 0; i < denemeler.size(); ++i) {
        st::SgmAyar a;
        a.P1 = denemeler[i].P1;
        a.P2 = denemeler[i].P2;
        a.yonSayisi = denemeler[i].yon;
        cv::Mat d;
        double t = st::sureOlc([&] { d = st::sgm(v.sol, v.sag, a); });
        st::satirYaz("Kendi SGM " + denemeler[i].ad, t, st::degerlendir(d, v.gt));
        cv::Mat renkli = st::renklendir(d);
        cv::putText(renkli, "P1=" + std::to_string(a.P1) + " P2=" + std::to_string(a.P2) + " yon=" +
                    std::to_string(a.yonSayisi), cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.55,
                    cv::Scalar(255, 255, 255), 2);
        cv::imwrite(ciktiKlasor + "/g4_sgm_P1_" + std::to_string(a.P1) + "_P2_" + std::to_string(a.P2) + "_yon_" +
                    std::to_string(a.yonSayisi) + ".png", renkli);
        if (i < 5) gorseller.push_back(renkli);
    }

    const int blok = 5;
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, st::MAX_DISP, blok);
    sgbm->setP1(8 * blok * blok);
    sgbm->setP2(32 * blok * blok);
    sgbm->setDisp12MaxDiff(1);
    sgbm->setUniquenessRatio(10);
    cv::Mat s16;
    double ts = st::sureOlc([&] { sgbm->compute(v.sol, v.sag, s16); });
    cv::Mat sDisp = st::opencvDisp(s16);
    st::satirYaz("OpenCV StereoSGBM", ts, st::degerlendir(sDisp, v.gt));

    cv::Mat gtRenkli = st::renklendir(v.gt);
    cv::putText(gtRenkli, "Gercek fark", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.55, cv::Scalar(255, 255, 255), 2);
    gorseller.push_back(gtRenkli);

    cv::Mat ust, alt, panel;
    cv::hconcat(std::vector<cv::Mat>(gorseller.begin(), gorseller.begin() + 3), ust);
    cv::hconcat(std::vector<cv::Mat>(gorseller.begin() + 3, gorseller.end()), alt);
    cv::vconcat(ust, alt, panel);
    cv::imwrite(ciktiKlasor + "/g4_panel.png", panel);

    if (argc > 3 && std::string(argv[3]) == "--goster") {
        cv::imshow("Gorev 4", panel);
        cv::waitKey(0);
    }
    return 0;
}
