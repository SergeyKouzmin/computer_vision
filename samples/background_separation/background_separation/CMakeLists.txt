cmake_minimum_required(VERSION 2.8) 

#set(static_lib_suffix ".a")
#set(shared_lib_suffix ".so")

#set(opencv_lib_dir ${opencv_dir}/${lib_subdir_dbg})

#include_directories(${ext_libs_dir}/opencv/2.3.1/include)


add_executable(bs_sample main.cpp)
target_link_libraries(bs_sample opencv_imgproc opencv_legacy opencv_highgui opencv_video opencv_core)

