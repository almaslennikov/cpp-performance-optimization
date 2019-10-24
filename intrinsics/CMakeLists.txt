cmake_minimum_required(VERSION 3.9.1)
project(filtering_vectorize)

set(SOURCES employee_registry.cpp filter_loader.cpp)

add_library(intrinsicslib ${SOURCES})
add_executable(intrinsics main.cpp)

set_target_properties(intrinsics PROPERTIES CXX_STANDARD 17)
set_target_properties(intrinsicslib PROPERTIES CXX_STANDARD 17)

target_compile_options(intrinsicslib PRIVATE "$<$<C_COMPILER_ID:GNU>:-mavx2>" -O2)

target_link_libraries(intrinsics PRIVATE intrinsicslib ${Ittnotity_LIBRARIES})
target_include_directories(intrinsics PRIVATE ${Ittnotity_INCLUDE_DIRS})
