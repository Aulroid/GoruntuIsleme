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
    int min_r = 10, max_r = 50;
    int num_r = max_r - min_r + 1;

    // Sin ve Cos Look-up Table
    std::vector<double> cos_tbl(360), sin_tbl(360);
    for (int theta = 0; theta < 360; ++theta) {
        double rad = theta * CV_PI / 180.0;
        cos_tbl[theta] = std::cos(rad);
        sin_tbl[theta] = std::sin(rad);
    }

    // 3D Accumulator Array: [rows][cols][radii]
    std::vector<std::vector<std::vector<int>>> accum(
        rows, std::vector<std::vector<int>>(cols, std::vector<int>(num_r, 0)));

    // Oylama
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

    // Cemberleri Esik Degere Gore Cizdirme
    int threshold = 180;
    cv::Mat result = img.clone();
    for (int b = 0; b < rows; ++b) {
        for (int a = 0; a < cols; ++a) {
            for (int r_idx = 0; r_idx < num_r; ++r_idx) {
                if (accum[b][a][r_idx] >= threshold) {
                    int radius = r_idx + min_r;
                    cv::circle(result, cv::Point(a, b), radius, cv::Scalar(0, 255, 255), 2);
                }
            }
        }
    }

    cv::imwrite("output_custom_hough_circles.jpg", result);
    std::cout << "Gorev 4 tamamlandi." << std::endl;
    return 0;
}