/**
 * @file feature.cpp
 * @author Yuan Zhao (zhao.yuan2@northeastern.edu)
 * @brief main entry for feature detection using different algorithms
 * @version 0.1
 * @date 2024-03-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <iostream>

// Define a function to process video frames and extract features using different algorithms.
// @param capture The video capture object to read frames from.
void processFeatures(cv::VideoCapture& capture) {
    cv::Mat frame, grayFrame;
    std::string currentFeature = "SURF"; // Default feature detection method

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            std::cerr << "No more frames to capture." << std::endl;
            break;
        }

        cv::cvtColor(frame, grayFrame, cv::COLOR_BGR2GRAY);
        std::vector<cv::KeyPoint> keypoints;

        if (currentFeature == "HARRIS") {
            // Implement Harris corner detection
            cv::Mat harrisCorners;
            cv::cornerHarris(grayFrame, harrisCorners, 2, 3, 0.04);
            // Thresholding to detect corners
            double harrisThreshold = 125.0;
            for (int i = 0; i < harrisCorners.rows; ++i) {
                for (int j = 0; j < harrisCorners.cols; ++j) {
                    if (harrisCorners.at<float>(i, j) > harrisThreshold) {
                        keypoints.emplace_back(cv::KeyPoint(float(j), float(i), 1.f));
                    }
                }
            }
        } else if (currentFeature == "SHI-TOMASI") {
            // Shi-Tomasi corner detection
            goodFeaturesToTrack(grayFrame, keypoints, 100, 0.01, 10);
        } else if (currentFeature == "SIFT") {
            // SIFT feature detection
            auto sift = cv::SIFT::create();
            sift->detect(grayFrame, keypoints);
        } else if (currentFeature == "SURF") {
            // SURF feature detection
            auto surf = cv::xfeatures2d::SURF::create(400);
            surf->detect(grayFrame, keypoints);
        }

        cv::Mat outputImage;
        cv::drawKeypoints(frame, keypoints, outputImage, cv::Scalar::all(-1), cv::DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

        cv::imshow("Feature Detection", outputImage);

        int key = cv::waitKey(30);
        if (key == 'q') break; // Quit on 'q'
        switch (key) {
            case 'h': currentFeature = "HARRIS"; break;
            case 's': currentFeature = "SHI-TOMASI"; break;
            case 'i': currentFeature = "SIFT"; break;
            case 'u': currentFeature = "SURF"; break;
        }
    }
}

// The main function initializes video capture and calls processFeatures to start processing.
int main() {
    cv::VideoCapture capture(0); // Open default camera
    if (!capture.isOpened()) {
        std::cerr << "Failed to open video capture device." << std::endl;
        return -1;
    }

    processFeatures(capture);
    return 0;
}
