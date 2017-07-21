
cmake_minimum_required(VERSION 2.8)

project(ShellingViewer)

include_directories($ENV{MESHLIB1.0_PATH})

include_directories($ENV{EIGEN_PATH})
file(GLOB SRC
    "*.h"
    "*.cpp"
)
add_executable(ShellingViewer ${SRC})