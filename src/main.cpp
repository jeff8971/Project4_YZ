

#include <iostream>
#include "chessboard.hpp"



int main(){
    cv::VideoCapture capture(0); // Open default camera. Use a different index if necessary.

    if (!capture.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }

    // Set the chessboard size
    cv::Size patternSize(9, 6);
    std::vector<cv::Point2f> corner_set; // to store the detected corners
    std::vector<std::vector<cv::Point2f>> corner_list; // to store the detected corners for all images
    std::vector<std::vector<cv::Vec3f>> object_list; // to store the 3D coordinates of the corners for all images
    




}