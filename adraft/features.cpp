#include "features.hpp"

void harris_corner_detection(const cv::Mat frame) {
    cv::Mat gray_frame;
    cv::cvtColor(frame, gray_frame, cv::COLOR_BGR2GRAY);
    cv::Mat dst, dst_norm, dst_norm_scaled;
    dst = cv::Mat::zeros(frame.size(), CV_32FC1);

    int block_size = 2;
    int aperture_size = 3;
    double k = 0.04;
    cv::cornerHarris(gray_frame, dst, block_size, aperture_size, k, cv::BORDER_DEFAULT);
    cv::normalize(dst, dst_norm, 0, 255, cv::NORM_MINMAX, CV_32FC1, cv::Mat());
    cv::convertScaleAbs(dst_norm, dst_norm_scaled);

    for (int i = 0; i < dst_norm.rows; i++) {
        for (int j = 0; j < dst_norm.cols; j++) {
            if ((int) dst_norm.at<float>(i, j) > 200) {
                cv::circle(frame, cv::Point(j, i), 5, cv::Scalar(0, 0, 255), 2, 8, 0);
            }
        }
    }
}

void draw_features(cv::Mat &frame) {
    // Create an ORB detector
    cv::Ptr<cv::ORB> orb_detector = cv::ORB::create();

    // Draw detected keypoints
    std::vector<cv::KeyPoint> keypoints;
    orb_detector->detect(frame, keypoints);

    // Draw detected keypoints
    cv::drawKeypoints(frame, keypoints, frame, cv::Scalar::all(-1), cv::DrawMatchesFlags::DEFAULT);
}