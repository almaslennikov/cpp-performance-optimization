cmake_minimum_required(VERSION 3.9.1)
project(filtering_baseline)

file(GLOB SOURCES ./*.cpp ./*.hpp)

add_executable(baseline main.cpp ${SOURCES})

set_target_properties(baseline PROPERTIES CXX_STANDARD 17)

target_link_libraries(baseline PRIVATE ${Ittnotity_LIBRARIES})
target_include_directories(baseline PRIVATE ${Ittnotity_INCLUDE_DIRS})
