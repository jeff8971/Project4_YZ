#include <iostream>
#include <opencv2/opencv.hpp>
#include "calibrate_utils.hpp"
#include "csv_utils.h"


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
    
    cv::Mat frame, camera_matrix, dist_coeffs;
    double reProjectionError = 0.0;
    cv::Size refS((int) capture.get(cv::CAP_PROP_FRAME_WIDTH), (int) capture.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::Size chessboard_size(CORNER_PER_ROW, CORNER_PER_COL); // Set the chessboard size

    std::vector<cv::Vec3f> point_set; // To store the detected corners
    for (int i = 0; i < CORNER_PER_COL; i++) {
        for (int j = 0; j < CORNER_PER_ROW; j++) {
            point_set.push_back(cv::Vec3f(j, -i, 0.0f));
        }
    }

    std::vector<std::vector<cv::Point2f>> corner_list; // The list of corner locations
    std::vector<std::vector<cv::Vec3f>> point_list; // The list of 3D points
    
    int img_counter = 0; // Counter to keep track of saved images

    while (true){
        capture >> frame;
        if (frame.empty()) {
            std::cerr << "Error reading frame, restart again." << std::endl;
            break;
        }

        std::vector<cv::Point2f> corner_set;
        bool chessboard_found = detect_chessboard(frame, chessboard_size, corner_set);

        // display the calibrate frame
        cv::imshow("Calibrate", frame);

        // key press
        char key = cv::waitKey(1);
        if (key == 's') {
            if (chessboard_found) {
                corner_list.push_back(corner_set);
                point_list.push_back(point_set);
                std::cout << "Calibration image and corners saved " << corner_set.size() << std::endl;
            } else {
                std::cerr << "Chessboard not found, cannot save the image" << std::endl;
            }

            std::string calibration_imageFile = "calibration_image_" + std::to_string(img_counter++) + ".png";

            if (cv::imwrite(calibration_imageFile, frame)) {
                std::cout << "Saved " << calibration_imageFile << std::endl;
            } else {
                std::cerr << "Failed to save " << calibration_imageFile << std::endl;
            }
        } else if (key == 'c') {
            if (corner_list.size() >= 5) {
                reProjectionError = camera_calibrate(point_list, corner_list, refS, camera_matrix, dist_coeffs);
                
                std::cout << "Camera matrix (Calibrated): \n" << std::endl << camera_matrix << std::endl;   
                std::cout << "Distortion coefficients (Calibrated): \n" << std::endl << dist_coeffs << std::endl;
                std::cout << "Reprojection error: \n" << std::endl << reProjectionError << std::endl;

                std::vector<double> camera_matrix_data;
                save_camera_calibration(camera_matrix_data, camera_matrix, dist_coeffs);

                std::string calibration_file_path = "/Users/jeff/Desktop/Project4_YZ/data/calibrate.csv";
                append_object_data_csv(calibration_file_path, "calibration", camera_matrix_data, true);

            } else {
                std::cerr << "Not enough images to calibrate the camera, need at least 5" << std::endl;
            }
        } else if (key == 'q' || key == 27) {
            break;
        }

    }

    capture.release();
    cv::destroyAllWindows();

    return 0;
}
