cmake_minimum_required(VERSION 3.9.1)
project(filtering_specify_string_type)

file(GLOB SOURCES ./*.cpp ./*.hpp)

add_executable(specify_string_type main.cpp ${SOURCES})

set_target_properties(specify_string_type PROPERTIES CXX_STANDARD 17)

target_link_libraries(specify_string_type PRIVATE ${Ittnotity_LIBRARIES})
target_include_directories(specify_string_type PRIVATE ${Ittnotity_INCLUDE_DIRS})
