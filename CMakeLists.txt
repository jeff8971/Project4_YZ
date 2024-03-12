# Specify the minimum version for CMake
cmake_minimum_required(VERSION 3.12)

# Project's name and specified language as C++
project(Project4_YZ LANGUAGES CXX)

# Set the C++ standard to C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Set the output directories for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Include directories from the 'include' directory within the project
include_directories(${CMAKE_SOURCE_DIR}/include)

# Automatically find and configure OpenCV or other libraries if needed
find_package(OpenCV REQUIRED)

# Task: calibration
# Add the executable that uses threshold_process.cpp and thresholding.cpp
add_executable(chessboard ${CMAKE_SOURCE_DIR} ./src/chessboard.cpp  ./src/main.cpp)
# Link with OpenCV libraries
target_link_libraries(chessboard PRIVATE ${OpenCV_LIBS})
# Ensure the OpenCV include directories are available to the target
target_include_directories(chessboard PRIVATE ${OpenCV_INCLUDE_DIRS})
