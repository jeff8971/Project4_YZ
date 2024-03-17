#ifndef FILE_UTILS_HPP
#define FILE_UTILS_HPP

#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>

int load_calibrate_file(const std::string &file_path, cv::Mat &camera_matrix, cv::Mat &dist_coeffs);


#endif // FILE_UTILS_HPP