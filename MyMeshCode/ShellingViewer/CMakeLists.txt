 project(ShellingViewer)

include_directories($ENV{MESHLIB1.0_PATH})

include_directories($ENV{EIGEN_PATH})

add_executable(ShellingViewer ShellingViewer.cpp)