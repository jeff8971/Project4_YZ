#include <iostream>
#include <opencv2/opencv.hpp>
#include "chessboard.hpp"
#include "camera_calibrate.hpp"
#include "file_utils.hpp"


int main() {
    cv::VideoCapture capture(1); // Open default camera. Use a different index if necessary.

    if (!capture.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }
    
    cv::Mat frame,
            camera_matrix = cv::Mat::eye(3, 3, CV_64F),
            dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);


    cv::Size patternSize(9, 6); // Set the chessboard size
    std::vector<cv::Point2f> corner_set; // To store the detected corners
    std::vector<std::vector<cv::Point2f>> corner_list; // To store the detected corners for all images
    std::vector<cv::Vec3f> object_set; // To store the 3D coordinates of the corners
    std::vector<std::vector<cv::Vec3f>> object_list; // To store the 3D coordinates for all images
    cv:: Mat rvecs, tvecs;

    std::string calibration_file_path = "/Users/jeff/Desktop/Project4_YZ/bin/calibrate.csv";

    // Load the camera calibration parameters
    if (load_calibrate_file(calibration_file_path, camera_matrix, dist_coeffs) == 0) {
        std::cout << "Camera matrix: " << std::endl << camera_matrix << std::endl;
        std::cout << "Distortion coefficients: " << std::endl << dist_coeffs << std::endl;
    } else {
        std::cerr << "Failed to load camera calibration parameters" << std::endl;
    }


    bool chessboard_detected = false;
    bool solvePnP = false;
    int saveCounter = 0; // Counter to keep track of saved images

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            std::cerr << "Error reading frame" << std::endl;
            break;
        }

        // Detect the chessboard corners
        bool patternFound = detectChessboard(frame, patternSize, corner_set);
        if (patternFound) {
            object_set.clear();
            for (int i = 0; i < patternSize.height; i++) {
                for (int j = 0; j < patternSize.width; j++) {
                    object_set.push_back(cv::Vec3f(j, -i, 0.0f));
                }
            }

            solvePnP = cv::solvePnP(object_set, corner_set, camera_matrix, dist_coeffs, rvecs, tvecs);

            if (!chessboard_detected){
                std::cout << "Number of Chessboard corner_set: " << corner_set.size() << std::endl;
                if (!corner_set.empty()) {
                    std::cout << "Corner coordinates: " << corner_set[0].x << ", " << corner_set[0].y << std::endl;
                }
            }
            chessboard_detected = true;
        } else {
            chessboard_detected = false;
        }

        cv::imshow("Chessboard", frame);
        char key = (char)cv::waitKey(30);
        if (key == 's' && patternFound) {
            corner_list.push_back(corner_set);
            std::vector<cv::Vec3f> object_set;
            for (int i = 0; i < patternSize.height; i++) {
                for (int j = 0; j < patternSize.width; j++) {
                    object_set.push_back(cv::Vec3f(j, -i, 0.0f));
                }
            }
            object_list.push_back(object_set);
            // Save the current frame
            std::string calibration_imageFile = "calibration_image_" + std::to_string(saveCounter++) + ".png";
            if (cv::imwrite(calibration_imageFile, frame)) {
                std::cout << "Saved " << calibration_imageFile << std::endl;
            } else {
                std::cerr << "Failed to save " << calibration_imageFile << std::endl;
            }
            std::cout << "Calibration image and corners saved " << corner_set.size() << std::endl;
        }  else if (key == 'q' || key == 27) {
            break;
        }
    }

    return 0;
}
