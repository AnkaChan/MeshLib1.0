cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(CMakeDemoForEigen)

include_directories($ENV{EIGEN_PATH})
include_directories($ENV{MESHLIB_PATH})
add_executable(main main.cpp)
