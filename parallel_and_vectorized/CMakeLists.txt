cmake_minimum_required(VERSION 3.9.1)
project(filtering_parallel_and_vectorized)

file(GLOB SOURCES ./*.cpp ./*.hpp)

add_executable(parallel_and_vectorized main.cpp ${SOURCES})

set_target_properties(parallel_and_vectorized PROPERTIES CXX_STANDARD 17)

target_link_libraries(parallel_and_vectorized PRIVATE ${Ittnotity_LIBRARIES})
target_include_directories(parallel_and_vectorized PRIVATE ${Ittnotity_INCLUDE_DIRS})
