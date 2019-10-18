cmake_minimum_required(VERSION 3.9.1)
project(filtering_parallel_baseline)

file(GLOB SOURCES ./*.cpp ./*.hpp)

add_executable(parallel_baseline main.cpp ${SOURCES})

set_target_properties(parallel_baseline PROPERTIES CXX_STANDARD 17)

target_link_libraries(parallel_baseline PRIVATE ${Ittnotity_LIBRARIES})
target_include_directories(parallel_baseline PRIVATE ${Ittnotity_INCLUDE_DIRS})
