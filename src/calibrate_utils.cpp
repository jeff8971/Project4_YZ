#include "calibrate_utils.hpp"

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

// calibrate the camera
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