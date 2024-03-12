// CameraCalibrator.hpp
#ifndef CAMERACALIBRATOR_HPP
#define CAMERACALIBRATOR_HPP

#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class CameraCalibrator {
public:
    CameraCalibrator();
    void addChessboardCorners(const std::vector<cv::Point2f>& corners, const cv::Vec3f& boardSize);
    bool calibrate(const cv::Size& imageSize);
    bool saveCalibrationData(const std::string& filename) const;

private:
    std::vector<std::vector<cv::Point2f>> corner_list;
    std::vector<std::vector<cv::Vec3f>> point_list;
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;
    double reProjectionError;
};

#endif // CAMERACALIBRATOR_HPP
