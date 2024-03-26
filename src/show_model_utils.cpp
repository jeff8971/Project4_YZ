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

int load_calibrate(std::vector<double> vec, cv::Mat &camera_matrix, cv::Mat &distCoeffs){

    if (vec.empty()) {
        printf("error: vector is empty.\n");
        return (-1);
    }

    // init the camera matrix and distortion coefficients
    camera_matrix = cv::Mat::zeros(3, 3, CV_64F);
    distCoeffs = cv::Mat::zeros(1, 14, CV_64F);

    // convert the vector to camera matrix
    for (int i = 0; i < camera_matrix.cols; i++){
        for (int j = 0; j < camera_matrix.rows; j++){
        camera_matrix.at<double>(j, i) = vec[i * camera_matrix.rows + j];
        }
    }

    // convert the vector to distortion coefficients
    for (int i = 0; i < distCoeffs.cols; i++){
        distCoeffs.at<double>(0, i) = vec[camera_matrix.rows * camera_matrix.cols + i];
    }

    return (0);
}
  

// error handling for the input data
bool validateInputs(cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Mat frame) {
  return !(camera_matrix.empty() || dist_coeffs.empty() || frame.empty());
}

// define the 3D points of the object
std::vector<cv::Point3f> define_object_points() {
  std::vector<cv::Point3f> object_points = {
    // Chessboard corners
    cv::Point3f(0, 0, 0), cv::Point3f(8, 0, 0), cv::Point3f(8, -5, 0), cv::Point3f(0, -5, 0),
    // 3D axes
    cv::Point3f(2, 0, 0), cv::Point3f(0, -2, 0), cv::Point3f(0, 0, 2),
    // Rectangle around the chessboard
    cv::Point3f(-1, 1, 0), cv::Point3f(9, 1, 0), cv::Point3f(9, -6, 0), cv::Point3f(-1, -6, 0)
  };
  return object_points;
}

// project the 3D points to the image plane
std::vector<cv::Point2f> project_points_to_image(std::vector<cv::Point3f> object_points, cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec) {
  std::vector<cv::Point2f> image_points;
  cv::projectPoints(object_points, rvec, tvec, camera_matrix, dist_coeffs, image_points);
  return image_points;
}


// 
void draw_image_on_frame(cv::Mat frame, std::vector<cv::Point2f> image_points) {
// Rectangle masking the chessboard
  std::vector<cv::Point> square_corners = {
    image_points[7],
    image_points[8],
    image_points[9],
    image_points[10]
  };


  cv::polylines(frame, std::vector<std::vector<cv::Point>>{square_corners}, true, cv::Scalar(255, 255, 255), 2);
  cv::fillPoly(frame, std::vector<std::vector<cv::Point>>{square_corners}, cv::Scalar(255, 255, 255));

  // Draw the four outside corners of the chessboard as circles
  for (int i = 0; i < 4; i++) {
    cv::circle(frame, image_points[i], 6, cv::Scalar(0, 0, 0), -1);
  }
  
  // Draw the 3D axes at the origin of the chessboard
  cv::line(frame, image_points[0], image_points[4], cv::Scalar(0, 0, 255), 3); // X-axis in red
  cv::line(frame, image_points[0], image_points[5], cv::Scalar(0, 255, 0), 3); // Y-axis in green
  cv::line(frame, image_points[0], image_points[6], cv::Scalar(255, 0, 0), 3); // Z-axis in blue
}

int draw_axis(cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec, cv::Mat frame) {
  if (!validateInputs(camera_matrix, dist_coeffs, frame)) {
    std::cerr << "Error: Invalid inputs." << std::endl;
    return -1;
  }

  auto objectPoints = define_object_points();
  auto image_points = project_points_to_image(objectPoints, camera_matrix, dist_coeffs, rvec, tvec);
  draw_image_on_frame(frame, image_points);

  return 0;
}



int draw_object(cv::Mat camera_matrix, cv::Mat dist_coeffs, cv::Vec3d rvec, cv::Vec3d tvec, std::vector<cv::Point3f> vertices, std::vector<std::vector<int>> faces, cv::Mat &frame){
  // check if the camera matrix, distortion coefficients, vertices, faces, and frame are empty
  if (camera_matrix.empty() || dist_coeffs.empty() || vertices.empty() || faces.empty() || frame.empty()){
    printf("error: camera matrix, distortion coefficients, vertices, faces, or frame is empty.\n");
    return (-1);
  }

  // project the vertices to the image plane
  std::vector<cv::Point2f> image_points;
  cv::projectPoints(vertices, rvec, tvec, camera_matrix, dist_coeffs, image_points);

  // draw the faces of the object
  for (int i = 0; i < faces.size(); i++){
    // map the z coordinate of the face to a color
    float z = (vertices[faces[i][0]].z + vertices[faces[i][1]].z + vertices[faces[i][2]].z) / 3;
    int color = (int)(z / 3.5 * 155) + 100;
    cv::Scalar faceColor(color, color, color);

    // draw the face
    for (int j = 0; j < 3; j++){
      cv::line(frame, image_points[faces[i][j]], image_points[faces[i][(j + 1) % 3]], faceColor, 3);
    }
  }

  return (0);
}