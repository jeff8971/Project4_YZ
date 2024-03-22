#include "show_model_utils.hpp"



int load_model_obj(std::string &file_path, std::vector<cv::Point3f> &vertices, std::vector<std::vector<int>> &faces){
    // Open the file
    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Error opening the file." << std::endl;
        return -1;
    }

    std::string line;
    while(std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }

        std::string first_word;
        std::stringstream ss(line);
        ss >> first_word;

        if (first_word == "v") {
            float x, y, z;
            ss >> x >> y >> z;

            if (ss.fail()) {
                std::cerr << "Error reading vertex data." << std::endl;
                return -1;
            }

            // Create a vertex at the center of the chessboard
            cv::Point3f vertex(x + 4, y - 2.5, z);

            // Add the vertex to the list of vertices
            vertices.push_back(vertex);
        }


        else if (first_word == "f") {
            std::vector<int> face;

            int v1, v2, v3;
            ss >> v1 >> v2 >> v3;

            if (ss.fail()) {
                std::cerr << "Error reading face data." << std::endl;
                return -1;
            }

            // Add the face to the list of faces
            face.push_back(v1 - 1);
            face.push_back(v2 - 1);
            face.push_back(v3 - 1);


            faces.push_back(face);
        }
        else {
            std::cout << "error: invalid line type" << first_word << std::endl;
            return -1;
        }
    }

    file.close();
    return 0;

}



std::vector<cv::Point3f> set_axes_points() {
    std::vector<cv::Point3f> axes_points;

    // Define the 3D points for the axes
    axes_points.push_back(cv::Point3f(0, 0, 0));  // Origin
    axes_points.push_back(cv::Point3f(3, 0, 0));  // X-axis
    axes_points.push_back(cv::Point3f(0, 3, 0));  // Y-axis
    axes_points.push_back(cv::Point3f(0, 0, -3));  // Z-axis

    return axes_points;
}


bool inside_rectangle(const cv::Point2f &p, const std::vector<cv::Point2f> &rectangle) {
    double area_rect = cv::contourArea(rectangle);
    double area_range = cv::contourArea(std::vector<cv::Point2f>{rectangle[0], rectangle[1], p}) +
                    cv::contourArea(std::vector<cv::Point2f>{rectangle[1], rectangle[2], p}) +
                    cv::contourArea(std::vector<cv::Point2f>{rectangle[2], rectangle[3], p}) +
                    cv::contourArea(std::vector<cv::Point2f>{rectangle[3], rectangle[0], p});
    
    return std::abs(area_rect - area_range) < 1e-3;

}


// Draw Virtual Reality model
void draw_vr_model(cv::Mat &frame,
                    const std::vector<cv::Point2f> &corner_set, 
                    const cv::Size& chessboard_size,
                    const cv::Mat &camera_matrix, 
                    const cv::Mat &dist_coeffs, 
                    const cv::Mat &rvec, 
                    const cv::Mat &tvec) {

    std::vector<cv::Point2f> chessboard_bound = {
        corner_set.front(),
        corner_set[chessboard_size.width - 1],
        corner_set.back(),
        corner_set[chessboard_size.width * (chessboard_size.height - 1)]
    };

    // Define the 3D coordinates of the object
    float base_size = 0.5f;
    float height = 0.5f;


    // chessboard, 9x6
    float cb_center_x = chessboard_size.width / 2.0f - 0.5f;
    float cb_center_y = - (chessboard_size.height / 2.0f - 0.5f);

    float offset_x = cb_center_x;
    float offset_y = cb_center_y;


    std::vector<cv::Point3f> object_set = {
        cv::Point3f(-0.5f + offset_x, -0.5f + offset_y, 0.0f),
        cv::Point3f(0.5f + offset_x, -0.5f + offset_y, 0.0f),
        cv::Point3f(0.5f + offset_x, 0.5f + offset_y, 0.0f),
        cv::Point3f(-0.5f + offset_x, 0.5f + offset_y, 0.0f),
    };


    // Project the 3D points to the image plane
    std::vector<cv::Point2f> img_sets;
    cv::projectPoints(object_set, rvec, tvec, camera_matrix, dist_coeffs, img_sets);

    for (const auto &img_point : img_sets) {
        if (!inside_rectangle(img_point, chessboard_bound)) {
            // make the object smaller
            base_size *= 0.75f;
            height *= 0.75f;

            object_set = {
                cv::Point3f(-base_size + offset_x, -base_size + offset_y, 0.0f),
                cv::Point3f(base_size + offset_x, -base_size + offset_y, 0.0f),
                cv::Point3f(base_size + offset_x, base_size + offset_y, 0.0f),
                cv::Point3f(-base_size + offset_x, base_size + offset_y, 0.0f),
                cv::Point3f(offset_x, offset_y, height)
            };
        }
        cv::projectPoints(object_set, rvec, tvec, camera_matrix, dist_coeffs, img_sets);
        break;
    }

    // Draw the object
    for (int i = 0; i < 4; i++) {
        cv::line(frame, img_sets[i], img_sets[(i + 1) % 4], cv::Scalar(0, 0, 255), 5);
        cv::line(frame, img_sets[i], img_sets[4], cv::Scalar(0, 0, 255), 5);
    }

    for (int i = 0; i < 4; i++) {
        cv::line(frame, img_sets[i], img_sets[(i + 1) % 4], cv::Scalar(0, 0, 255), 8);
    }

}



int load_calibrate_file(std::vector<double> &camera_matrix_data, cv::Mat &camera_matrix, cv::Mat &dist_coeffs) {
    
    // check if the vector has data or not
    if (camera_matrix_data.empty()) {
        std::cerr << "Camera matrix data is empty." << std::endl;
        return -1;
    }
    
    camera_matrix = cv::Mat::zeros(3, 3, CV_64F);
    dist_coeffs = cv::Mat::zeros(1, 14, CV_64F);

    // load the camera matrix and distortion coefficients
    for (int i = 0; i < camera_matrix.cols; i++) {
        for (int j = 0; j < camera_matrix.rows; j++) {
            camera_matrix.at<double>(j, i) = camera_matrix_data[i * camera_matrix.rows + j];
        }
    }

    // load the distortion coefficients
    for (int i = 0; i < dist_coeffs.cols; i++) {
        dist_coeffs.at<double>(0, i) = camera_matrix_data[i + camera_matrix.cols * camera_matrix.rows];
    }

    return 0;
}

void print_menu_calibrate() {
    std::cout << "Select an option:" << std::endl;
    std::cout << "Press 's' to save a calibration image. (make sure not less than 5)" << std::endl;
    std::cout << "Press 'c' to perform calibration." << std::endl;
}



void print_menu_show_model() {
    std::cout << "Select an option:" << std::endl;
    std::cout << "Press 'p' to print board's pose." << std::endl;
    std::cout << "Press 'k' to display the virtual object persistently on the chessboard." << std::endl;
    std::cout << "Press 'f' to display a robust feature on the chessboard." << std::endl;
    std::cout << "Press 'q' to exit." << std::endl;
}

void load_3D_axes(const cv::Mat &frame, const cv::Mat &camera_matrix, const cv::Mat &dist_coeffs, const cv::Mat &rvec, const cv::Mat &tvec) {
    // 3D points of the axes
    std::vector<cv::Point3f> axes_points = {
            {0, 0, 0}, // origin
            {1, 0, 0}, // x-axis
            {0, 1, 0}, // y-axis
            {0, 0, -1}  // z-axis
    };
    // Project the 3D points to the image plane
    std::vector<cv::Point2f> image_points;
    cv::projectPoints(axes_points, rvec, tvec, camera_matrix, dist_coeffs, image_points);

    // Draw the axes
    cv::line(frame, image_points[0], image_points[1], cv::Scalar(0, 0, 255), 5); // x-axis
    cv::line(frame, image_points[0], image_points[2], cv::Scalar(0, 255, 0), 5); // y-axis
    cv::line(frame, image_points[0], image_points[3], cv::Scalar(255, 0, 0), 5); // z-axis
}