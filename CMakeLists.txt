cmake_minimum_required(VERSION 3.14)
project(RayTracer)

#setup external libs-----------------------------
include(loadExternal.cmake)
include(utils.cmake)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src/geometry)
add_subdirectory(src/utility)
add_subdirectory(examples)
add_subdirectory(tests)

add_library(raytracer src/raytracer.cpp)
target_link_libraries(raytracer PRIVATE geometry)
target_include_directories(raytracer PUBLIC ./include)
