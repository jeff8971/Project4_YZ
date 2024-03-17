// CameraCalibrator.cpp
#include "camera_calibrate.hpp"
#include <fstream>
#include <iostream>

void camera_calibrate(const std::vector<std::vector<cv::Point2f>>& corner_list, 
                    const std::vector<std::vector<cv::Vec3f>>& object_list, 
                    const cv::Size& image_size, 
                    cv::Mat& camera_matrix, 
                    cv::Mat& dist_coeffs){

    if (corner_list.size() >= 5) {
        camera_matrix = cv::Mat::eye(3, 3, CV_64F);
        camera_matrix.at<double>(0, 2) = image_size.width / 2;  
        camera_matrix.at<double>(1, 2) = image_size.height / 2;

        dist_coeffs = cv::Mat::zeros(8, 1, CV_64F);
        std::vector<cv::Mat> rvecs, tvecs;
        double reProjectionError = cv::calibrateCamera(object_list, corner_list, image_size, camera_matrix, dist_coeffs, rvecs, tvecs);

        std::cout << "Camera matrix: " << std::endl << camera_matrix << std::endl;   
        std::cout << "Distortion coefficients: " << std::endl << dist_coeffs << std::endl;
        std::cout << "Reprojection error: " << std::endl << reProjectionError << std::endl;

                        
    }
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