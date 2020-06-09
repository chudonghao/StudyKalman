cmake_minimum_required(VERSION 3.10)
project(StudyKalman)

set(CMAKE_CXX_STANDARD 11)

########################################################
# cmake configure
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

########################################################
# kalman
include_directories(kalman/include)

########################################################
# Qt5
find_package(Qt5 REQUIRED COMPONENTS Widgets)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

########################################################
# Eigen3
find_package(Eigen3 REQUIRED)
message(STATUS EIGEN3_DEFINITIONS:${EIGEN3_DEFINITIONS})
message(STATUS EIGEN3_INCLUDE_DIRS:${EIGEN3_INCLUDE_DIRS})
message(STATUS Eigen3_DIR:${Eigen3_DIR})
include("${Eigen3_DIR}/UseEigen3.cmake")

add_subdirectory(src)