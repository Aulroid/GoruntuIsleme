#include <opencv2/opencv.hpp>
#include <iostream>

int main() {
    cv::Mat img = cv::imread("input.jpg");
    if (img.empty()) return -1;

    cv::Mat gray, filtered;
    cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    cv::medianBlur(gray, filtered, 5);

    std::vector<cv::Vec3f> circles;
    cv::HoughCircles(filtered, circles, cv::HOUGH_GRADIENT, 1,
                     filtered.rows / 8, 100, 30, 10, 100);

    cv::Mat result = img.clone();
    for (size_t i = 0; i < circles.size(); i++) {
        cv::Vec3i c = circles[i];
        cv::Point center = cv::Point(c[0], c[1]);
        int radius = c[2];
        cv::circle(result, center, radius, cv::Scalar(255, 0, 255), 3);
        cv::circle(result, center, 2, cv::Scalar(0, 255, 0), 3);
    }

    cv::imwrite("output_hough_circles.jpg", result);
    std::cout << "Gorev 3 tamamlandi." << std::endl;
    return 0;
}