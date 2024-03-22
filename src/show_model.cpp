
#include "csv_utils.h"
#include "show_model_utils.hpp"
#include "features.hpp"

#include <iostream>
#include <opencv2/opencv.hpp>
#include <sstream>
#include <fstream>
#include <atomic>
#include <regex>
#include <vector>
#include <thread>



int main(int argc, char** argv) {
    
    std::string calibrate_file_path = "/Users/jeff/Desktop/Project4_YZ/bin/calibrate.csv";


    // load the camera calibration parameters from csv file
    std::vector<std::string> calibrate_labels;
    std::vector<std::vector<double>> features_data;
    read_object_data_csv(calibrate_file_path, calibrate_labels, features_data);

    // convert the calibrate data to matrices
    cv::Mat camera_matrix;
    cv::Mat dist_coeffs;
    load_calibrate_file(features_data[0], camera_matrix, dist_coeffs);

    // load the object model data
    std::vector<cv::Point3f> vertices;
    std::vector<std::vector<int>> faces;
    std::string model_file_path = "/Users/jeff/Desktop/Project4_YZ/bin/teapot.obj";
    load_model_obj(model_file_path, vertices, faces);

    // create vector of 3D points
    std::vector<cv::Vec3f> point_set;
    for (int i = 0; i < CORNER_PER_COL; i++) {
        for (int j = 0; j < CORNER_PER_ROW; j++) {
            point_set.push_back(cv::Vec3f(j, -i, 0.0f));
        }
    }

    















    cv::VideoCapture capture(1); // Open default camera. Use a different index if necessary.

    if (!capture.isOpened()) {
        std::cerr << "Error opening video capture" << std::endl;
        return -1;
    }
    
    cv::Mat frame;

    std::thread key_input_t(capture_key_input);

    cv::Size chessboard_size(9, 6); // Set the chessboard size
    std::vector<cv::Point2f> corner_set; // To store the detected corners
    std::vector<std::vector<cv::Point2f>> corner_list; // To store the detected corners for all images
    std::vector<cv::Vec3f> point_set; // To store the 3D coordinates of the corners
    std::vector<std::vector<cv::Vec3f>> point_list; // To store the 3D coordinates for all images
    
    cv:: Mat camera_matrix = cv::Mat::eye(3, 3, CV_64F);
    cv:: Mat dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);
    cv:: Mat rvec, tvec;

    std::vector<cv::Point3f> axes_points = set_axes_points();

    std::string calibration_file_path = "/Users/jeff/Desktop/Project4_YZ/bin/calibrate.csv";

    // Load the camera calibration parameters, make sure the calibrate is done before running this program
    if (load_calibrate_file(calibration_file_path, camera_matrix, dist_coeffs) == 0) {
        std::cout << "Camera matrix: " << std::endl << camera_matrix << std::endl;
        std::cout << "Distortion coefficients: " << std::endl << dist_coeffs << std::endl;
    } else {
        std::cerr << "Failed to load camera calibration parameters" << std::endl;
    }

    // Load 3D model
    std::vector<Vertex> vertices;
    std::vector<Face> faces;
    std::vector<Normal> normals;
    std::vector<TextureCoord> texture_coord;
    std::string model_file_path = "/Users/jeff/Desktop/Project4_YZ/bin/teapot.obj";
    
    if (load_model_obj(model_file_path, vertices, texture_coord, normals, faces) == 0) {
        std::cout << "Model loaded" << std::endl;
    } else {
        std::cerr << "Failed to load model" << std::endl;
    }

    bool chessb_first_found = false; // To keep track of the first time the chessboard is found
    bool display_3D_axes = false; // Flag to display the 3D axes
    bool display_vr_model = false; // Flag to display the VR model
    bool solvePnP = false; // Flag to solve PnP successfully or not
    int img_save_counter = 0; // Counter to keep track of saved images

    print_menu_show_model();

    while (true) {
        capture >> frame;
        if (frame.empty()) {
            std::cerr << "Error reading frame" << std::endl;
            break;
        }

        // Detect the chessboard corners
        bool chessboard_found = detect_chessboard(frame, chessboard_size, corner_set);
        if (chessboard_found) {
            point_set.clear();
            for (int i = 0; i < chessboard_size.height; i++) {
                for (int j = 0; j < chessboard_size.width; j++) {
                    point_set.push_back(cv::Vec3f(j, -i, 0.0f));
                }
            }

        // Solve PnP
        solvePnP = cv::solvePnP(point_set, corner_set, camera_matrix, dist_coeffs, rvec, tvec);

        if (!chessb_first_found){
                std::cout << "Number of Chessboard corner_set: " << corner_set.size() << std::endl;
                if (!corner_set.empty()) {
                    std::cout << "Corner coordinates: " << corner_set[0].x << ", " << corner_set[0].y << std::endl;
                }
        }
        chessb_first_found = true;
        } 
        else {
            chessb_first_found = false;
        }
        
        // Display the 3D axes on the chessboard
        if (chessboard_found && display_3D_axes && solvePnP) {
            std::vector<cv::Point2f> img_points;
            cv::projectPoints(point_set, rvec, tvec, camera_matrix, dist_coeffs, img_points);

            cv::line(frame, img_points[0], img_points[1], cv::Scalar(0, 0, 255), 2);
            cv::line(frame, img_points[0], img_points[2], cv::Scalar(0, 255, 0), 2);
            cv::line(frame, img_points[0], img_points[3], cv::Scalar(255, 0, 0), 2);
        }


        char key = char (cv::waitKey(100));
        
        if (key == 'k') {
            keep_display_vr.store(true);
        } else if (key == 'p') {
            display_3D_axes = !display_3D_axes;
        } else if (key == 'o') {
            display_vr_model = !display_vr_model;
        } else if (key == 'f'){
            display_features = !display_features;
        } else if (key == 'r' && solvePnP) {
            std::cout << "Rotation Vector:\n " << rvec << std::endl;
            std::cout << "Translation Vector:\n " << tvec << std::endl;
        }
        
        
        if (keep_display_vr.load() && solvePnP) {
            if (chessboard_found) {
                draw_vr_model(frame, corner_set, chessboard_size, camera_matrix, dist_coeffs, rvec, tvec);
            }
        }

        if (display_features.load()) {
            draw_features(frame);
        }

        // Display the frame
        cv::imshow("Chessboard", frame);
        cv::waitKey(1);

        if (key == 'q' || key == 27) {
            break;
        }
    
    }
    /*
    if (key_input_t.joinable()) {
        key_input_t.join();
    }
    */
   
    capture.release();
    cv::destroyAllWindows();


    return 0;
}
