// CameraCalibrator.cpp
#include "CameraCalibrator.hpp"
#include <fstream>
#include <iostream>

CameraCalibrator::CameraCalibrator() : reProjectionError(0.0) {
    cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
    distCoeffs = cv::Mat::zeros(8, 1, CV_64F);
}

void CameraCalibrator::addChessboardCorners(const std::vector<cv::Point2f>& corners, const cv::Vec3f& boardSize) {
    corner_list.push_back(corners);
    // Generate the corresponding object points for the chessboard
    std::vector<cv::Vec3f> objectPoints;
    for (int i = 0; i < boardSize[1]; ++i) {
        for (int j = 0; j < boardSize[0]; ++j) {
            objectPoints.push_back(cv::Vec3f(j, i, 0.0f));
        }
    }
    point_list.push_back(objectPoints);
}

bool CameraCalibrator::calibrate(const cv::Size& imageSize) {
    if (corner_list.size() >= 5) {
        // Setting the principal point at the center
        cameraMatrix.at<double>(0, 2) = imageSize.width / 2;
        cameraMatrix.at<double>(1, 2) = imageSize.height / 2;

        std::vector<cv::Mat> rvecs, tvecs;
        std::vector<std::vector<cv::Point3f>> objectPoints;
        // Convert from Vec3f to Point3f for calibrateCamera
        for (auto& objPts : point_list) {
            std::vector<cv::Point3f> points;
            for (auto& pt : objPts) {
                points.push_back(cv::Point3f(pt[0], pt[1], pt[2]));
            }
            objectPoints.push_back(points);
        }

        reProjectionError = cv::calibrateCamera(objectPoints, corner_list, imageSize, cameraMatrix, distCoeffs, rvecs, tvecs);
        std::cout << "Camera matrix: " << cameraMatrix << std::endl;
        std::cout << "Distortion coefficients: " << distCoeffs << std::endl;
        std::cout << "Re-projection error: " << reProjectionError << std::endl;
        return true;
    }
    return false;
}

bool CameraCalibrator::saveCalibrationData(const std::string& filename) const {
    std::ofstream outFile(filename);
    if (!outFile) {
        return false;
    }

    outFile << "Camera Matrix:\n" << cameraMatrix << std::endl;
    outFile << "Distortion Coefficients:\n" << distCoeffs << std::endl;
    outFile << "Re-Projection Error: " << reProjectionError << std::endl;

    return true;
}
