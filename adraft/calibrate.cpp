#include <iostream>
#include <opencv2/opencv.hpp>
#include "calibrate_utils.hpp"
#include "file_utils.hpp"
#include "csv_util.h"


#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <fstream>
#include <atomic>
#include <regex>
#include <vector>
#include <thread>



int main(int argc, char** argv) {
    cv::VideoCapture capture(1); // Open default camera. Use a iphone for external camera.

    if (!capture.isOpened()) {
        std::cerr << "Error: can not opening video capture" << std::endl;
        return -1;
    }
    
    cv::Mat frame;
    cv::Size refS((int) capture.get(cv::CAP_PROP_FRAME_WIDTH), (int) capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::Size chessboard_size(CORNER_PER_ROW, CORNER_PER_COL); // Set the chessboard size



    std::vector<cv::Point2f> corner_set; // To store the detected corners
    std::vector<std::vector<cv::Point2f>> corner_list; // To store the detected corners for all images
    std::vector<cv::Vec3f> object_point; // To store the 3D coordinates of the corners
    std::vector<std::vector<cv::Vec3f>> point_list; // To store the 3D coordinates for all images
    
    cv:: Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
    cv:: Mat dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);
    cv:: Mat rvec, tvec;


    std::string calibration_file_path = "/Users/jeff/Desktop/Project4_YZ/bin/calibrate.csv";

    bool chessboard_detected = false;
    bool chessb_first_found = false; // To keep track of the first time the chessboard is found
    int saveCounter = 0; // Counter to keep track of saved images

    print_menu_calibrate();

    while (true) {
        capture >> frame;
        // error check
        if (frame.empty()) {
            std::cerr << "Error reading frame, restart again." << std::endl;
            break;
        }

        // Detect the chessboard corners
        bool chessboard_found = detect_chessboard(frame, chessboard_size, corner_set);
        if (chessboard_found) {
            object_point.clear();
            for (int i = 0; i < chessboard_size.height; i++) {
                for (int j = 0; j < chessboard_size.width; j++) {
                    object_point.push_back(cv::Vec3f(j, -i, 0.0f));
                }
            }

            if (!chessb_first_found) {
                std::cout << "Number of Chessboard corner_set: " << corner_set.size() << std::endl;
                if (!corner_set.empty()) {
                    std::cout << "Corner coordinates: " << corner_set[0].x << ", " << corner_set[0].y << std::endl;
                }
            }
            chessb_first_found = true;
        } else {
            chessb_first_found = false;
        }



        // key input to save the image and calibrate the camera

        char key = key_input.load();

        if (key == ' ') {
            // 's' for save the image and save the calibrate data
            if (key == 's' && chessboard_found) {
                corner_list.push_back(corner_set);
                std::vector<cv::Vec3f> point_set;

                for (int i = 0; i < chessboard_size.height; i++) {
                    for (int j = 0; j < chessboard_size.width; j++) {
                        point_set.push_back(cv::Vec3f(j, -i, 0.0f));
                    }
                }

                point_list.push_back(point_set);
                std::cout << "Calibration image and corners saved " << corner_set.size() << std::endl;
            } else if (key == 'z') {
                // Save the current frame
                std::string calibration_imageFile = "calibration_image_" + std::to_string(saveCounter++) + ".png";
                if (cv::imwrite(calibration_imageFile, frame)) {
                    std::cout << "Saved " << calibration_imageFile << std::endl;
                } else {
                    std::cerr << "Failed to save " << calibration_imageFile << std::endl;
                }
            } else if (key == 'c') {
                if (corner_list.size() >= 5) {
                    std::vector<cv::Mat> rvec, tvec;

                    double reProjectionError = cv::calibrateCamera(point_list, corner_list, frame.size(), camera_matrix, dist_coeffs, rvec, tvec, cv::CALIB_FIX_ASPECT_RATIO);
                    std::cout << "Camera matrix: " << std::endl << camera_matrix << std::endl;
                    std::cout << "Distortion coefficients: " << std::endl << dist_coeffs << std::endl;
                
                    if (save_camera_calibration(calibration_file_path, camera_matrix, dist_coeffs, reProjectionError) == 0) {
                        std::cout << "Camera calibration parameters saved to "<< calibration_file_path << std::endl;
                    } else{
                        std::cerr << "Failed to save camera calibration parameters" << std::endl;
                    }
                }
                else {
                    std::cerr << "Not enough images to calibrate the camera, need at least 5" << std::endl;
                }

            }

            key_input.store(' '); // Reset the key input

        }
        
        // Frame to show
        cv::imshow("Calibrate", frame);
        cv::waitKey(1);
        if (key == 'q' || key == 27) {
            break;
        }
    }

    if (key_input_t.joinable()) {
        key_input_t.join();
    }

    capture.release();
    cv::destroyAllWindows();

    return 0;
}


// Save the camera calibration parameters to a file
int save_camera_calibration(const std::string& filename, 
                            const cv::Mat& camera_matrix, 
                            const cv::Mat& dist_coeffs,
                            double reProjectionError){

    std::ofstream camera_calibrate_file(filename);
    if (!camera_calibrate_file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return -1;
    }

    camera_calibrate_file << "Camera matrix: " << std::endl << camera_matrix << std::endl;
    camera_calibrate_file << "Distortion coefficients: " << std::endl << dist_coeffs << std::endl;
    camera_calibrate_file << "Reprojection error: " << std::endl << reProjectionError << std::endl;

    camera_calibrate_file.close();
    return 0;
}
