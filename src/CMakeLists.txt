cmake_minimum_required(VERSION 2.8)
set (CMAKE_CXX_STANDARD 14)
project( image_stitching )
find_package( OpenCV REQUIRED )

add_executable( image_stitching 
image_stitching.cpp 
brisk_descriptor.h 
brisk_descriptor.cpp 
cylindrical.h 
cylindrical.cpp
)

target_link_libraries( image_stitching ${OpenCV_LIBS} )
