cmake_minimum_required(VERSION 3.9.1)
project(filtering_vectors_of_simple_objects)

file(GLOB SOURCES ./*.cpp ./*.hpp)

add_executable(vectors_of_simple_objects main.cpp ${SOURCES})

set_target_properties(vectors_of_simple_objects PROPERTIES CXX_STANDARD 17)

target_link_libraries(vectors_of_simple_objects PRIVATE ${Ittnotity_LIBRARIES})
target_include_directories(vectors_of_simple_objects PRIVATE ${Ittnotity_INCLUDE_DIRS})
