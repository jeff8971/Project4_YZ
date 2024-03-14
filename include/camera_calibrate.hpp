// Camera_calibrator.hpp
#ifndef CAMERA_CALIBRATE_HPP
#define CAMERA_CALIBRATE_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

void camera_calibrate(const std::vector<std::vector<cv::Point2f>>& corner_list, 
                    const std::vector<std::vector<cv::Vec3f>>& object_list, 
                    const cv::Size& image_size, 
                    cv::Mat& camera_matrix, 
                    cv::Mat& dist_coeffs);

int save_camera_calibration(const std::string& filename, 
                            const cv::Mat& camera_matrix, 
                            const cv::Mat& dist_coeffs,
                            double reProjectionError);

#endif // CAMERACALIBRATOR_HPP
