/**
 * @file show_model.cpp
 * @author Yuan Zhao (zhao.yuan2@northeastern.edu)
 * @brief main entry for showing the model object
 * @version 0.1
 * @date 2024-03-19
 */

#include "csv_utils.h"
#include "show_model_utils.hpp"
#include "calibrate_utils.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <fstream>
#include <atomic>
#include <regex>
#include <vector>
#include <thread>


// main entry for showing the model object
int main(int argc, char** argv) {
    
    std::string calibrate_file_path = "/Users/jeff/Desktop/Project4_YZ/data/calibrate.csv";

    // load the camera calibration parameters from csv file
    std::vector<std::string> calibrate_labels;
    std::vector<std::vector<double>> features_data;
    read_object_data_csv(calibrate_file_path, calibrate_labels, features_data);

    // convert the calibrate data to matrices
    cv::Mat camera_matrix;
    cv::Mat dist_coeffs;
    load_calibrate(features_data[0], camera_matrix, dist_coeffs);

    // load the object model data
    std::vector<cv::Point3f> vertices;
    std::vector<std::vector<int>> faces;
    std::string model_file_path = "/Users/jeff/Desktop/Project4_YZ/data/teapot.obj";
    load_model_obj(model_file_path, vertices, faces);

    // create vector of 3D points
    std::vector<cv::Vec3f> point_set;
    for (int i = 0; i < CORNER_PER_COL; i++) {
        for (int j = 0; j < CORNER_PER_ROW; j++) {
            point_set.push_back(cv::Vec3f(j, -i, 0.0f));
        }
    }

    // Turn on the camera

    cv::Mat frame;
    cv::VideoCapture capture(1); // Open default camera. Use a different index if necessary.

    cv::Size chessboard_size(CORNER_PER_ROW, CORNER_PER_COL);

    if (!capture.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }
    
    int image_counter = 0;
    while (true) {
        capture >> frame;
        if (frame.empty()) {
            std::cerr << "Error reading frame" << std::endl;
            break;
        }
        

        std::vector<cv::Point2f> corner_set;
        bool chessboard_found = detect_chessboard(frame, chessboard_size, corner_set);
        if (chessboard_found) {
            cv::Mat rvec, tvec;
            cv::solvePnP(point_set, corner_set, camera_matrix, dist_coeffs, rvec, tvec);

            // print out the pose of the chessboard
            std::cout << "rvec: " << rvec << std::endl;
            std::cout << "tvec: " << tvec << std::endl;

            // draw the axis on the frame
            draw_axis(camera_matrix, dist_coeffs, rvec, tvec, frame);

            // project the 3D points to the image
            // draw_object(camera_matrix, dist_coeffs, rvec, tvec, vertices, faces, frame);
        }


        // Display the frame
        cv::imshow("Show Model", frame);
        cv::waitKey(1);

        if (cv::waitKey(1) == 27 || cv::waitKey(1) == 'q'){
            break;
        } else if (cv::waitKey(1) == 's') {
            std::cout << "Saving image..." << std::endl;
            std::string image_name = "show_model_" + std::to_string(image_counter++) + ".png";
            cv::imwrite(image_name, frame);

            std::string image_path = "/Users/jeff/Desktop/Project4_YZ/bin/" + image_name;
            std::cout << "Image saved to: " << image_path << std::endl;
        }
    }

    capture.release();
    cv::destroyAllWindows();
    
    return 0;
}