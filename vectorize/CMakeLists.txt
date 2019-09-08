cmake_minimum_required(VERSION 3.9.1)
project(filtering_vectorize)

file(GLOB SOURCES ./*.cpp ./*.hpp)

add_executable(vectorize main.cpp ${SOURCES})

set_target_properties(vectorize PROPERTIES CXX_STANDARD 17)

target_link_libraries(vectorize PRIVATE ${Ittnotity_LIBRARIES})
target_include_directories(vectorize PRIVATE ${Ittnotity_INCLUDE_DIRS})
