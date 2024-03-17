


#include "file_utils.hpp"



int load_calibrate_file(const std::string &file_path, cv::Mat &camera_matrix, cv::Mat &dist_coeffs) {
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << file_path << std::endl;
        return -1;
    }

    std::string line;

    while (std::getline(file, line)) {
            if (line.find("Camera Matrix:") != std::string::npos) {
                for (int i = 0; i < 3; ++i) {
                    std::getline(file, line); // Read the next line containing matrix row
                    std::stringstream ss(line);
                    char dummy; // Used to consume the '[' ',' ';' and ']' characters
                    if (i == 0) ss >> dummy; // Consume the '[' only for the first row
                    
                    for (int j = 0; j < 3; ++j) {
                        double value;
                        ss >> value >> dummy; // Read each value and the following comma/semicolon
                        camera_matrix.at<double>(i, j) = value;
                    }
                }
            } else if (line.find("Distortion Coefficients:") != std::string::npos) {
                std::getline(file, line); // Read the next line containing coefficients
                std::stringstream ss(line);
                char dummy; // Used to consume the '[' and ';' characters
                ss >> dummy; // Consume the '['
                
                for (int i = 0; i < 5; ++i) {
                    double value;
                    ss >> value >> dummy; // Read each value and the following semicolon
                    dist_coeffs.at<double>(i, 0) = value;
                }
            }
        }

  file.close();
  return 0; // Success
}