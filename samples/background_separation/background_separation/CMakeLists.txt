cmake_minimum_required(VERSION 2.8) 

set(static_lib_suffix ".a")
set(shared_lib_suffix ".so")

set(ext_libs_dir ./../../../external_libs)
#set(opencv_lib_dir ${ext_libs_dir}/lib64)

set(opencv_lib_dir /usr/local/lib)


include_directories(${ext_libs_dir}/include)

add_library(imgproc SHARED IMPORTED)
set_property(TARGET imgproc PROPERTY IMPORTED_LOCATION ${opencv_lib_dir}/libopencv_imgproc.so)

add_library(legacy SHARED IMPORTED)
set_property(TARGET legacy PROPERTY IMPORTED_LOCATION ${opencv_lib_dir}/libopencv_legacy.so)

add_library(highgui SHARED IMPORTED)
set_property(TARGET highgui PROPERTY IMPORTED_LOCATION ${opencv_lib_dir}/libopencv_highgui.so)

add_library(video SHARED IMPORTED)
set_property(TARGET video PROPERTY IMPORTED_LOCATION ${opencv_lib_dir}/libopencv_video.so)

add_library(core SHARED IMPORTED)
set_property(TARGET core PROPERTY IMPORTED_LOCATION ${opencv_lib_dir}/libopencv_core.so)

add_executable(background_separation main.cpp)
#set_target_properties(background_separation  PROPERTIES COMPILE_FLAGS "-c -pipe -O2 -Wall -W -fPIE")
set_target_properties(background_separation PROPERTIES COMPILE_FLAGS "-O2 -Wall -pthread -g -W -fPIE -pipe")
target_link_libraries(background_separation imgproc highgui video core)

