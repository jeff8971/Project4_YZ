/**
 * @file show_model_utils.cpp
 * @author Yuan Zhao (zhao.yuan2@northeastern.edu)
 * @brief Header file for model visualization utilities using OpenCV.
 *        Provides functionalities for loading calibration data and 3D models,
 *        validating inputs, defining 3D object points, projecting these points onto image planes,
 *        and drawing projected points or models on frames.
 * @version 0.1
 * @date 2024-03-19
 */

#ifndef SHOW_MODEL_UTILS_HPP
#define SHOW_MODEL_UTILS_HPP

#include <sstream>
#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>

/**
 * Loads a 3D model from an OBJ file.
 * 
 * @param file_path Path to the OBJ file.
 * @param vertices Output vector of model vertices.
 * @param faces Output vector of indices defining the model's faces.
 * @return int Returns 0 on success, -1 on failure (e.g., file not found, parsing error).
 */
int load_model_obj(std::string &file_path, std::vector<cv::Point3f> &vertices, std::vector<std::vector<int>> &faces);

/**
 * Loads calibration data into camera matrix and distortion coefficients.
 * 
 * @param vec A vector containing camera matrix and distortion coefficients data.
 * @param camera_matrix Output camera matrix.
 * @param distCoeffs Output distortion coefficients.
 * @return int Returns 0 on success, -1 if the input vector is empty.
 */
int load_calibrate(std::vector<double> vec, cv::Mat &cameraMatrix, cv::Mat &distCoeffs);

// error handling for the input data
bool validateInputs(cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat frame);

// define the 3D points of the object
std::vector<cv::Point3f> define_object_points();
// project the 3D points to the image plane
std::vector<cv::Point2f> project_points_to_image(std::vector<cv::Point3f> object_points, cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec);
// draw the 3D model on the frame
void draw_image_on_frame(cv::Mat frame, std::vector<cv::Point2f> image_points);
// draw the 3D axes on the frame
int draw_axis(cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec, cv::Mat frame);
// draw the 3D model on the frame
int draw_object(cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec, std::vector<cv::Point3f> vertices, std::vector<std::vector<int>> faces, cv::Mat &frame);
 
#endif 