#include "../ortak/stereo_ortak.hpp"

struct Yontem {
    std::string ad;
    st::BenzerlikFonk fonk;
};

int main(int argc, char** argv) {
    std::string veriKlasor = st::argVeya(argc, argv, 1, "veri");
    std::string ciktiKlasor = st::argVeya(argc, argv, 2, "sonuclar");
    std::filesystem::create_directories(ciktiKlasor);

    st::Veri v = st::yukle(veriKlasor);

    std::vector<Yontem> yontemler = {{"SAD", &st::sad}, {"SSD", &st::ssd}, {"ZNCC", &st::zncc}};

    cv::Mat sagParlak;
    v.sag.convertTo(sagParlak, CV_8U, 0.7, 40);

    st::BlokAyar ayar;
    ayar.r = 4;
    ayar.benzersizlik = 0.0f;

    std::vector<cv::Mat> normalSonuc, parlakSonuc;

    std::cout << "\n[1] Normal goruntu cifti, pencere 9x9" << std::endl;
    st::baslikYaz();
    for (const auto& y : yontemler) {
        cv::Mat d;
        double t = st::sureOlc([&] { d = st::blokEslestir(v.sol, v.sag, y.fonk, ayar); });
        st::satirYaz(y.ad, t, st::degerlendir(d, v.gt));
        normalSonuc.push_back(st::renklendir(d));
        cv::imwrite(ciktiKlasor + "/g3_" + y.ad + ".png", normalSonuc.back());
    }

    std::cout << "\n[2] Sag goruntu parlaklik/kontrast degisimi (0.7*I + 40), pencere 9x9" << std::endl;
    st::baslikYaz();
    for (const auto& y : yontemler) {
        cv::Mat d;
        double t = st::sureOlc([&] { d = st::blokEslestir(v.sol, sagParlak, y.fonk, ayar); });
        st::satirYaz(y.ad, t, st::degerlendir(d, v.gt));
        parlakSonuc.push_back(st::renklendir(d));
        cv::imwrite(ciktiKlasor + "/g3_" + y.ad + "_parlaklik.png", parlakSonuc.back());
    }

    std::cout << "\n[3] Pencere boyutu etkisi (kotu piksel %)" << std::endl;
    std::cout << std::left << std::setw(10) << "Pencere";
    for (const auto& y : yontemler) std::cout << std::right << std::setw(10) << y.ad;
    std::cout << std::endl;
    for (int r : {1, 2, 4, 7}) {
        st::BlokAyar a = ayar;
        a.r = r;
        std::cout << std::left << std::setw(10) << (std::to_string(2 * r + 1) + "x" + std::to_string(2 * r + 1));
        for (const auto& y : yontemler) {
            cv::Mat d = st::blokEslestir(v.sol, v.sag, y.fonk, a);
            std::cout << std::right << std::setw(9) << std::fixed << std::setprecision(2)
                      << st::degerlendir(d, v.gt).kotuPiksel << "%";
        }
        std::cout << std::endl;
    }

    cv::Mat ust, alt, panel;
    cv::hconcat(normalSonuc, ust);
    cv::hconcat(parlakSonuc, alt);
    cv::vconcat(ust, alt, panel);
    for (size_t i = 0; i < yontemler.size(); ++i) {
        cv::putText(panel, yontemler[i].ad, cv::Point(10 + 384 * int(i), 20), cv::FONT_HERSHEY_SIMPLEX, 0.6,
                    cv::Scalar(255, 255, 255), 2);
        cv::putText(panel, yontemler[i].ad + " (parlaklik)", cv::Point(10 + 384 * int(i), 308),
                    cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
    }
    cv::imwrite(ciktiKlasor + "/g3_panel.png", panel);

    if (argc > 3 && std::string(argv[3]) == "--goster") {
        cv::imshow("Gorev 3", panel);
        cv::waitKey(0);
    }
    return 0;
}
