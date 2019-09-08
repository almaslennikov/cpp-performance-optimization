cmake_minimum_required(VERSION 3.9.1)
project(filtering_no_unnecessary_copies)

file(GLOB SOURCES ./*.cpp ./*.hpp)

add_executable(no_unnecessary_copies main.cpp ${SOURCES})
set_target_properties(no_unnecessary_copies PROPERTIES CXX_STANDARD 17)

target_link_libraries(no_unnecessary_copies PRIVATE ${Ittnotity_LIBRARIES})
target_include_directories(no_unnecessary_copies PRIVATE ${Ittnotity_INCLUDE_DIRS})
