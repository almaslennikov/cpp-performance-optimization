cmake_minimum_required(VERSION 3.9.1)
project(filtering)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(Ittnotify REQUIRED)

add_subdirectory(baseline)
add_subdirectory(parallel_baseline)
add_subdirectory(no_unnecessary_copies)
add_subdirectory(vectors_of_simple_objects)
add_subdirectory(specify_string_type)
add_subdirectory(vectorize)
add_subdirectory(intrinsics)
add_subdirectory(parallel_and_vectorized)
