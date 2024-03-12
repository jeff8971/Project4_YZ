#include <iostream>
#include <opencv2/opencv.hpp>
#include "chessboard.hpp"

int main() {
    cv::VideoCapture capture(1); // Open default camera. Use a different index if necessary.

    if (!capture.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }

    cv::Size patternSize(9, 6); // Set the chessboard size
    std::vector<cv::Point2f> corner_set; // To store the detected corners
    std::vector<std::vector<cv::Point2f>> corner_list; // To store the detected corners for all images
    std::vector<cv::Vec3f> object_set; // To store the 3D coordinates of the corners
    std::vector<std::vector<cv::Vec3f>> object_list; // To store the 3D coordinates for all images
    
    // Set the 3D coordinates of the corners
    for (int i = 0; i < patternSize.height; i++) {
        for (int j = 0; j < patternSize.width; j++) {
            object_set.push_back(cv::Vec3f(j, -i, 0.0f));
        }
    }

    cv::Mat frame;
    int saveCounter = 0; // Counter to keep track of saved images

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            std::cerr << "Error reading frame" << std::endl;
            break;
        }

        // Detect the chessboard corners
        if (detectChessboard(frame, patternSize, corner_set)) {
            std::cout << "Number of Chessboard corner_set: " << corner_set.size() << std::endl;
            if (!corner_set.empty()) {
                // Draw the detected corners
                std::cout << "Corner coordinates: " << corner_set[0].x << ", " << corner_set[0].y << std::endl;
            }

        }

        cv::imshow("Chessboard", frame);
        char key = (char)cv::waitKey(30);
        if (key == 's') {
            // Save the detected corners, 3D coordinates, and the image
            corner_list.push_back(corner_set);
            object_list.push_back(object_set);
            
            // Save the current frame
            std::string filename = "calibration_image_" + std::to_string(saveCounter++) + ".png";
            if (cv::imwrite(filename, frame)) {
                std::cout << "Saved " << filename << std::endl;
            } else {
                std::cerr << "Failed to save " << filename << std::endl;
            }

            std::cout << "Calibration image and corners saved" << std::endl;
        } else if (key == 27) { // ESC key
            break;
        }
    }

    return 0;
}
