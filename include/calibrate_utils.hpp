/**
 * @file calibrate_utils.cpp
 * @author Yuan Zhao (zhao.yuan2@northeastern.edu)
 * @brief Header file for calibrate_utils.cpp. This file defines the functions used for detecting chessboard corners,
 *        calibrating the camera using the detected points, and saving the calibration data. It utilizes OpenCV for
 *        image processing and camera calibration functionalities.
 * @version 0.1
 * @date 2024-03-19
 */

#ifndef CALIBRATE_UTILS_HPP
#define CALIBRATE_UTILS_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>


// Detects chessboard corners in the given image.
// @param frame The image in which chessboard corners are to be detected.
// @param chessboard_size The size (number of inner corners per chessboard row and column) of the chessboard.
// @param corner_set A vector to store the detected corners.
// @return true if the chessboard is successfully detected; false otherwise.
bool detect_chessboard(cv::Mat &frame, cv::Size &chessboard_size, std::vector<cv::Point2f> &corner_set);

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
                      cv::Mat& dist_coeffs);


// Saves camera calibration data.
// @param calibration_data A vector to store the calibration data.
// @param camera_matrix The camera matrix obtained from calibration.
// @param dist_coeffs The distortion coefficients obtained from calibration.
// @return 0 on success, -1 if camera matrix or distortion coefficients are empty.
int save_camera_calibration(std::vector<double> calibration_data,
                            cv::Mat& camera_matrix, 
                            const cv::Mat& dist_coeffs);

#endif // CALIBRATE_UTILS_HPP