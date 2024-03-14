#include <iostream>
#include <opencv2/opencv.hpp>
#include "chessboard.hpp"
#include "camera_calibrate.hpp"

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
    
    std::string calibration_file_path = "/Users/jeff/Desktop/Project4_YZ/bin/calibrate.csv";
    bool chessboard_detected = false;

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
        } else if (key == 'c') {
            if (corner_list.size() >= 5) {
                std::vector<cv::Mat> rvecs, tvecs;
                double reProjectionError = cv::calibrateCamera(object_list, corner_list, frame.size(), camera_matrix, dist_coeffs, rvecs, tvecs, cv::CALIB_FIX_ASPECT_RATIO);
                std::cout << "Camera matrix: " << std::endl << camera_matrix << std::endl;
                std::cout << "Distortion coefficients: " << std::endl << dist_coeffs << std::endl;
                std::cout << "Reprojection error: " << std::endl << reProjectionError << std::endl;
            
                if (save_camera_calibration(calibration_file_path, camera_matrix, dist_coeffs, reProjectionError) == 0) {
                    std::cout << "Camera calibration parameters saved to "<< calibration_file_path << std::endl;
                } else{
                    std::cerr << "Failed to save camera calibration parameters" << std::endl;
                }
            }
            else {
                std::cerr << "Not enough images to calibrate the camera, need at least 5" << std::endl;
            }
        } else if (key == 'q') {
            // Quit the program
            break;
        }
        else if (key == 27) { // ESC key
            break;
        }
    }

    return 0;
}
