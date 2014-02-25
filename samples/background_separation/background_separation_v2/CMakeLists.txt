cmake_minimum_required(VERSION 2.8) 

set(static_lib_suffix ".a")
set(shared_lib_suffix ".so")

set(ext_libs_dir ./../../../external_libs)
set(opencv_lib_dir ${ext_libs_dir}/lib)

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


add_executable(blobtrack_sample blobtrack_sample.cpp)
target_link_libraries(blobtrack_sample imgproc legacy highgui video core)

