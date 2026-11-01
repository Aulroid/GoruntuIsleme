#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat img = cv::imread("input.jpg");
    if (img.empty()) {
        std::cerr << "Resim bulunamadi!" << std::endl;
        return -1;
    }

    cv::Mat gray, edges, filtered;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    
    // Canny Kenar Tespiti
    cv::Canny(gray, edges, 50, 150);
    
    // Median Filtre
    cv::medianBlur(edges, filtered, 3);

    // 1. Standart Hough Lines (Kırmızı)
    cv::Mat img_lines = img.clone();
    std::vector<cv::Vec2f> lines;
    cv::HoughLines(filtered, lines, 1, CV_PI / 180, 100);
    for (size_t i = 0; i < lines.size(); i++) {
        float rho = lines[i][0], theta = lines[i][1];
        double a = cos(theta), b = sin(theta);
        double x0 = a * rho, y0 = b * rho;
        cv::Point pt1(cvRound(x0 + 1000 * (-b)), cvRound(y0 + 1000 * (a)));
        cv::Point pt2(cvRound(x0 - 1000 * (-b)), cvRound(y0 - 1000 * (a)));
        cv::line(img_lines, pt1, pt2, cv::Scalar(0, 0, 255), 2);
    }

    // 2. Probabilistic Hough Lines (Yeşil)
    cv::Mat img_lines_p = img.clone();
    std::vector<cv::Vec4i> lines_p;
    cv::HoughLinesP(filtered, lines_p, 1, CV_PI / 180, 50, 50, 10);
    for (size_t i = 0; i < lines_p.size(); i++) {
        cv::Vec4i l = lines_p[i];
        cv::line(img_lines_p, cv::Point(l[0], l[1]), cv::Point(l[2], l[3]), cv::Scalar(0, 255, 0), 2);
    }

    cv::imwrite("output_hough_lines.jpg", img_lines);
    cv::imwrite("output_hough_lines_p.jpg", img_lines_p);
    std::cout << "Gorev 1 tamamlandi." << std::endl;
    return 0;
}