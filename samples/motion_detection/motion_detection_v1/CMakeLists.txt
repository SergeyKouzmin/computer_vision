cmake_minimum_required(VERSION 2.8) 


add_executable(motempl motempl.c)
target_link_libraries(motempl opencv_imgproc opencv_legacy opencv_highgui opencv_video opencv_core)

