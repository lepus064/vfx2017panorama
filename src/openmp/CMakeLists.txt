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

find_package(OpenMP)
if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

target_link_libraries( image_stitching ${OpenCV_LIBS} -fopenmp)
