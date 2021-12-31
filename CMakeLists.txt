cmake_minimum_required(VERSION 3.10)

project(opengl-utils)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)

add_library(opengl-utils "")

target_include_directories(opengl-utils PUBLIC
    ${CMAKE_HOME_DIRECTORY}/include)

add_subdirectory("src")

target_link_libraries(opengl-utils PUBLIC
    OpenGL::GL
    GLEW::GLEW)