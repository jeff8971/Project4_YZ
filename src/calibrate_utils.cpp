/**
 * @file calibrate_utils.cpp
 * @author Yuan Zhao (zhao.yuan2@northeastern.edu)
 * @brief This file defines the functions used for detecting chessboard corners, calibrating the camera using the detected points, and saving the calibration data. It utilizes OpenCV for image processing and camera calibration functionalities.
 * @version 0.1
 * @date 2024-03-19
 */

#include "calibrate_utils.hpp"
\
// Detects chessboard corners in the given image.
// @param frame The image in which chessboard corners are to be detected.
// @param chessboard_size The size (number of inner corners per chessboard row and column) of the chessboard.
// @param corner_set A vector to store the detected corners.
// @return true if the chessboard is successfully detected; false otherwise.
bool detect_chessboard(cv::Mat &frame, cv::Size &chessboard_size, std::vector<cv::Point2f> &corner_set){
    cv::Mat gray_image;
    cv::cvtColor(frame, gray_image, cv::COLOR_BGR2GRAY);
    bool chessboard_found = cv::findChessboardCorners(gray_image, chessboard_size, corner_set, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE);
    if (chessboard_found) {
        cv::cornerSubPix(gray_image, corner_set, cv::Size(5, 5), cv::Size(-1, -1), 
        cv::TermCriteria(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 30, 0.1));
        cv::drawChessboardCorners(frame, chessboard_size, corner_set, chessboard_found);
    }
    return chessboard_found;

}

// Calibrates the camera using the detected chessboard corners.
// @param point_list 3D points in real world space.
// @param corner_list 2D points in image plane.
// @param image_size Size of the image used for calibration.
// @param camera_matrix Output camera matrix.
// @param dist_coeffs Output vector of distortion coefficients.
// @return The re-projection error, which gives a measure of how exact the found parameters are.
double camera_calibrate(const std::vector<std::vector<cv::Vec3f>>& point_list, 
                      const std::vector<std::vector<cv::Point2f>>& corner_list, 
                      const cv::Size& image_size, 
                      cv::Mat& camera_matrix, 
                      cv::Mat& dist_coeffs){
    
    camera_matrix = cv::Mat::eye(3, 3, CV_64FC1);
    camera_matrix.at<double>(0, 2) = image_size.width / 2;  
    camera_matrix.at<double>(1, 2) = image_size.height / 2;

    dist_coeffs = cv::Mat::zeros(1, 14, CV_64FC1);

    std::cout << "Initial camera matrix: \n" << camera_matrix << std::endl;
    std::cout << "Initial distortion coefficients: \n" << dist_coeffs << std::endl;

    std::cout << "Calibrating camera..." << std::endl;
    std::cout << "Number of images for calibration: " << corner_list.size() << std::endl;

    std::vector<cv::Mat> rvecs, tvecs;
    cv::TermCriteria termCrit(cv::TermCriteria::COUNT | cv::TermCriteria::EPS, 30, 0.1);
    double reProjectionError = cv::calibrateCamera(point_list, corner_list, image_size, camera_matrix, dist_coeffs, rvecs, tvecs,
     cv::CALIB_FIX_ASPECT_RATIO | cv::CALIB_RATIONAL_MODEL, termCrit);

    return reProjectionError;

}

// Saves camera calibration data.
// @param calibration_data A vector to store the calibration data.
// @param camera_matrix The camera matrix obtained from calibration.
// @param dist_coeffs The distortion coefficients obtained from calibration.
// @return 0 on success, -1 if camera matrix or distortion coefficients are empty.
int save_camera_calibration(std::vector<double> calibration_data, cv::Mat& camera_matrix, const cv::Mat& dist_coeffs) {
  
  if (camera_matrix.empty() || dist_coeffs.empty()) {
    std::cerr << "Camera matrix or distortion coefficients are empty, please check" << std::endl;
    return -1;
  }

  // convert the camera matrix and distortion coefficients to a vector
  for (int i = 0; i < camera_matrix.rows; i++) {
    for (int j = 0; j < camera_matrix.cols; j++) {
      calibration_data.push_back(camera_matrix.at<double>(j, i));
    }
  }

  for (int i = 0; i < dist_coeffs.rows; i++) {
      calibration_data.push_back(dist_coeffs.at<double>(0, i));
  }
  
  return 0;
}