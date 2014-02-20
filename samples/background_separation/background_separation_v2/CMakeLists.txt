cmake_minimum_required(VERSION 2.8) 

#set(static_lib_suffix ".a")
#set(shared_lib_suffix ".so")

#set(opencv_lib_dir ${opencv_dir}/${lib_subdir_dbg})

#include_directories(${ext_libs_dir}/opencv/2.3.1/include)


add_executable(blobtrack_sample blobtrack_sample.cpp)
target_link_libraries(blobtrack_sample opencv_imgproc opencv_legacy opencv_highgui opencv_video opencv_core)

