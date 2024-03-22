#ifndef SHOW_MODEL_UTILS_HPP
#define SHOW_MODEL_UTILS_HPP

#include <vector>
#include <string>
#include <string>
#include <fstream>
#include <iostream>
#include <sstream>


#include <iostream>
#include <regex>
#include <vector>
#include <string>
#include <fstream>
#include <opencv2/opencv.hpp>

int load_calibrate_file(std::vector<double> &camera_matrix_data, cv::Mat &camera_matrix, cv::Mat &dist_coeffs);
   
void print_menu_calibrate();

void print_menu_show_model();


struct Vertex {
    float x, y, z;
};

struct TextureCoord {
    float u, v;
};

struct Normal {
    float nx, ny, nz;
};

struct Face {
    std::vector<int> vertex_indices;
    std::vector<int> texture_indices;
    std::vector<int> normal_indices;
};

int load_model_obj(std::string &file_path, std::vector<cv::Point3f> &vertices, std::vector<std::vector<int>> &faces);

std::vector<cv::Point3f> set_axes_points();

bool inside_rectangle(const cv::Point2f &p, const std::vector<cv::Point2f> &rectangle);

void draw_vr_model(cv::Mat &frame,
                    const std::vector<cv::Point2f> &corner_set, 
                    const cv::Size& chessboard_size,
                    const cv::Mat &camera_matrix, 
                    const cv::Mat &dist_coeffs, 
                    const cv::Mat &rvec, 
                    const cv::Mat &tvec);

#endif // LOAD_MODEL_HPP