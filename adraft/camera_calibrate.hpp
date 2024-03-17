#ifndef CAMERA_CALIBRATE_HPP
#define CAMERA_CALIBRATE_HPP

#include <opencv2/opencv.hpp>
#include <vector>


void calibrateCamera(const std::vector<std::vector<cv::Point2f>>& corner_list,
                    const std::vector<std::vector<cv::Vec3f>>& object_list, 
                    const cv::Size& frame_size, cv::Mat& camera_matrix, cv::Mat& dist_coeffs);



void saveCameraCalibration(const std::string& filename, const cv::Mat& camera_matrix, const cv::Mat& dist_coeffs, double error);

#endif // CAMERA_CALIBRATE_HPP