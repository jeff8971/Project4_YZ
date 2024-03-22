#ifndef CALIBRATE_UTILS_HPP
#define CALIBRATE_UTILS_HPP

#include <opencv2/opencv.hpp>
#include <vector>
#include <string>



bool detect_chessboard(cv::Mat &frame, cv::Size &chessboard_size, std::vector<cv::Point2f> &corner_set);


double camera_calibrate(const std::vector<std::vector<cv::Vec3f>>& point_list, 
                      const std::vector<std::vector<cv::Point2f>>& corner_list, 
                      const cv::Size& image_size, 
                      cv::Mat& camera_matrix, 
                      cv::Mat& dist_coeffs);

int save_camera_calibration(std::vector<double> calibration_data,
                            cv::Mat& camera_matrix, 
                            const cv::Mat& dist_coeffs);

#endif // CALIBRATE_UTILS_HPP