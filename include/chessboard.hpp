
#ifndef CHESSBOARD_HPP
#define CHESSBOARD_HPP

#include <opencv2/opencv.hpp>
#include <vector>

bool detectChessboard(cv::Mat &image, cv::Size &boardSize, std::vector<cv::Point2f> &corner_set);


#endif // CHESSBOARD_HPP
