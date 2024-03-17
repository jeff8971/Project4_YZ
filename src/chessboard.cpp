// calibrate.cpp
#include "chessboard.hpp"

bool detectChessboard(cv::Mat &image, cv::Size &patternSize, std::vector<cv::Point2f> &corner_set){
    // covert the image to grayscale
    cv::Mat gray;
    cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);

    // find the chessboard corners
    bool patternFound = cv::findChessboardCorners(gray, patternSize, corner_set);

    if (patternFound && corner_set.size() > 0){
        // refine the corner positions
        cv::cornerSubPix(gray, corner_set, cv::Size(11, 11), cv::Size(-1, -1),
         cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));

        // draw the corners on the image
        cv::drawChessboardCorners(image, patternSize, corner_set, patternFound);
    }
    return patternFound;
}