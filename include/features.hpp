#ifndef FEATURES_HPP
#define FEATURES_HPP

#include <opencv2/opencv.hpp>
#include <vector>


void harris_corner_detection(const cv::Mat frame);

void draw_features(cv::Mat &frame);

#endif // FEATURES_HPP