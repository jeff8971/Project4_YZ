#ifndef SHOW_MODEL_UTILS_HPP
#define SHOW_MODEL_UTILS_HPP

#include <sstream>
#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>

int load_calibrate(std::vector<double> vec, cv::Mat &cameraMatrix, cv::Mat &distCoeffs);

int load_model_obj(std::string &file_path, std::vector<cv::Point3f> &vertices, std::vector<std::vector<int>> &faces);

bool validateInputs(cv::Mat cameraMatrix, cv::Mat distCoeffs, cv::Mat frame);

std::vector<cv::Point3f> define_object_points();

std::vector<cv::Point2f> project_points_to_image(std::vector<cv::Point3f> object_points, cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec);
  
void draw_image_on_frame(cv::Mat frame, std::vector<cv::Point2f> image_points);

int draw_axis(cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec, cv::Mat frame);

int draw_object(cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec, std::vector<cv::Point3f> vertices, std::vector<std::vector<int>> faces, cv::Mat &frame);
 
#endif 