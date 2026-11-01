#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <cmath>

int main() {
    cv::Mat img = cv::imread("input.jpg");
    if (img.empty()) return -1;

    cv::Mat gray, edges, filtered;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::Canny(gray, edges, 50, 150);
    cv::medianBlur(edges, filtered, 3);

    int rows = filtered.rows;
    int cols = filtered.cols;
    int min_r = 10, max_r = 60;
    int num_r = max_r - min_r + 1;

    std::vector<double> cos_tbl(360), sin_tbl(360);
    for (int t = 0; t < 360; ++t) {
        cos_tbl[t] = std::cos(t * CV_PI / 180.0);
        sin_tbl[t] = std::sin(t * CV_PI / 180.0);
    }

    // 3D Akumulator
    std::vector<std::vector<std::vector<int>>> accum(
        rows, std::vector<std::vector<int>>(cols, std::vector<int>(num_r, 0)));

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < cols; ++x) {
            if (filtered.at<uchar>(y, x) > 0) {
                for (int r = min_r; r <= max_r; ++r) {
                    int r_idx = r - min_r;
                    for (int theta = 0; theta < 360; ++theta) {
                        int a = static_cast<int>(std::round(x - r * cos_tbl[theta]));
                        int b = static_cast<int>(std::round(y - r * sin_tbl[theta]));
                        if (a >= 0 && a < cols && b >= 0 && b < rows) {
                            accum[b][a][r_idx]++;
                        }
                    }
                }
            }
        }
    }

    // Es merkezli cember tespiti: Ayni (a, b) merkezinde birden fazla yaricapta oy var mi?
    cv::Mat result = img.clone();
    int min_circle_votes = 120; // Tek cember esik oyu

    for (int b = 0; b < rows; ++b) {
        for (int a = 0; a < cols; ++a) {
            int concentric_count = 0;
            std::vector<int> detected_radii;

            for (int r_idx = 0; r_idx < num_r; ++r_idx) {
                if (accum[b][a][r_idx] >= min_circle_votes) {
                    concentric_count++;
                    detected_radii.push_back(r_idx + min_r);
                }
            }

            // Oylama matrisi seviyesinde es merkezlilik kontrolu (En az 2 farkli yaricapta oy)
            if (concentric_count >= 2) {
                for (int r : detected_radii) {
                    cv::circle(result, cv::Point(a, b), r, cv::Scalar(0, 165, 255), 2);
                }
                cv::circle(result, cv::Point(a, b), 2, cv::Scalar(255, 0, 0), 3);
            }
        }
    }

    cv::imwrite("output_concentric_circles.jpg", result);
    std::cout << "Gorev 5 tamamlandi." << std::endl;
    return 0;
}