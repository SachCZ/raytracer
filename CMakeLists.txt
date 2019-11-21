cmake_minimum_required(VERSION 3.0)
project(RayTracer)

#setup external libs-----------------------------
include(loadExternal.cmake)
include(utils.cmake)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src/geometry)
add_subdirectory(src/utility)
add_subdirectory(src/physics)

option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    add_subdirectory(tests/unit)
    add_subdirectory(tests/profile)
endif(BUILD_TESTS)

add_library(raytracer src/raytracer.cpp)
target_link_libraries(raytracer PUBLIC geometry utility physics)
target_include_directories(raytracer PUBLIC ./include)
