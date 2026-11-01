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

    int width = filtered.cols;
    int height = filtered.rows;
    int max_rho = static_cast<int>(std::hypot(width, height));
    int num_rho = 2 * max_rho + 1;
    int num_theta = 180;

    // Sinus ve Cosinus degerlerini bellek tespiti (Look-up Table)
    std::vector<double> cos_tbl(num_theta), sin_tbl(num_theta);
    for (int t = 0; t < num_theta; ++t) {
        double rad = t * CV_PI / 180.0;
        cos_tbl[t] = std::cos(rad);
        sin_tbl[t] = std::sin(rad);
    }

    // Oylama Matrisi (Accumulator Matrix)
    cv::Mat accumulator = cv::Mat::zeros(num_rho, num_theta, CV_32SC1);

    // Oylama Islemi
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (filtered.at<uchar>(y, x) > 0) {
                for (int t = 0; t < num_theta; ++t) {
                    int rho = static_cast<int>(std::round(x * cos_tbl[t] + y * sin_tbl[t])) + max_rho;
                    accumulator.at<int>(rho, t)++;
                }
            }
        }
    }

    // Akumulator Matrisini Resim Olarak Kaydetme
    cv::Mat accum_vis;
    double max_val;
    cv::minMaxLoc(accumulator, NULL, &max_val);
    accumulator.convertTo(accum_vis, CV_8UC1, 255.0 / max_val);
    cv::imwrite("accumulator_lines.jpg", accum_vis);

    // Esik Degere Gore Dogrulari Cizdirme
    int threshold = 100;
    cv::Mat result = img.clone();
    for (int r = 0; r < num_rho; ++r) {
        for (int t = 0; t < num_theta; ++t) {
            if (accumulator.at<int>(r, t) >= threshold) {
                int rho = r - max_rho;
                double a = cos_tbl[t], b = sin_tbl[t];
                double x0 = a * rho, y0 = b * rho;
                cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
                cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
                cv::line(result, pt1, pt2, cv::Scalar(255, 0, 0), 2);
            }
        }
    }

    cv::imwrite("output_custom_hough_lines.jpg", result);
    std::cout << "Gorev 2 tamamlandi." << std::endl;
    return 0;
}