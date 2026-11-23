#include "../ortak/stereo_ortak.hpp"

int main(int argc, char** argv) {
    std::string veriKlasor = st::argVeya(argc, argv, 1, "veri");
    std::string ciktiKlasor = st::argVeya(argc, argv, 2, "sonuclar");
    std::filesystem::create_directories(ciktiKlasor);

    st::Veri v = st::yukle(veriKlasor);

    cv::Ptr<cv::StereoBM> bm = cv::StereoBM::create(st::MAX_DISP, 15);
    bm->setPreFilterCap(31);
    bm->setTextureThreshold(10);
    bm->setUniquenessRatio(15);
    bm->setSpeckleWindowSize(100);
    bm->setSpeckleRange(32);

    const int blok = 5;
    cv::Ptr<cv::StereoSGBM> sgbm = cv::StereoSGBM::create(0, st::MAX_DISP, blok);
    sgbm->setP1(8 * blok * blok);
    sgbm->setP2(32 * blok * blok);
    sgbm->setUniquenessRatio(10);
    sgbm->setSpeckleWindowSize(100);
    sgbm->setSpeckleRange(2);
    sgbm->setDisp12MaxDiff(1);
    sgbm->setMode(cv::StereoSGBM::MODE_SGBM);

    cv::Mat bm16, sgbm16;
    double bmSure = st::sureOlc([&] { bm->compute(v.sol, v.sag, bm16); }, 10);
    double sgbmSure = st::sureOlc([&] { sgbm->compute(v.sol, v.sag, sgbm16); }, 10);

    cv::Mat bmDisp = st::opencvDisp(bm16);
    cv::Mat sgbmDisp = st::opencvDisp(sgbm16);

    st::baslikYaz();
    st::satirYaz("StereoBM (15x15)", bmSure, st::degerlendir(bmDisp, v.gt));
    st::satirYaz("StereoSGBM (5x5)", sgbmSure, st::degerlendir(sgbmDisp, v.gt));

    cv::Mat gtRenkli = st::renklendir(v.gt);
    cv::Mat bmRenkli = st::renklendir(bmDisp);
    cv::Mat sgbmRenkli = st::renklendir(sgbmDisp);

    cv::Mat bmGri;
    bm16.convertTo(bmGri, CV_8U, 255.0 / (st::MAX_DISP * 16.0));
    cv::Mat sgbmGri;
    sgbm16.convertTo(sgbmGri, CV_8U, 255.0 / (st::MAX_DISP * 16.0));

    cv::imwrite(ciktiKlasor + "/g1_stereobm.png", bmRenkli);
    cv::imwrite(ciktiKlasor + "/g1_stereosgbm.png", sgbmRenkli);
    cv::imwrite(ciktiKlasor + "/g1_stereobm_gri.png", bmGri);
    cv::imwrite(ciktiKlasor + "/g1_stereosgbm_gri.png", sgbmGri);
    cv::imwrite(ciktiKlasor + "/g1_gercek_fark.png", gtRenkli);

    cv::Mat ust, alt, panel;
    cv::hconcat(v.solRenkli, gtRenkli, ust);
    cv::hconcat(bmRenkli, sgbmRenkli, alt);
    cv::vconcat(ust, alt, panel);
    cv::putText(panel, "Sol (col1)", cv::Point(10, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(panel, "Gercek fark", cv::Point(394, 20), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(panel, "StereoBM", cv::Point(10, 308), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::putText(panel, "StereoSGBM", cv::Point(394, 308), cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    cv::imwrite(ciktiKlasor + "/g1_panel.png", panel);

    if (argc > 3 && std::string(argv[3]) == "--goster") {
        cv::imshow("Gorev 1", panel);
        cv::waitKey(0);
    }
    return 0;
}
