cmake_minimum_required(VERSION 3.1)
project(RayTracer)

#setup external libs-----------------------------
include(cmake/loadExternal.cmake)
include(GoogleTest)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src/geometry)
add_subdirectory(src/utility)
add_subdirectory(src/physics)

option(BUILD_TESTS "Build unit tests" OFF)

enable_testing()

if(BUILD_TESTS)
    add_subdirectory(tests/unit)
    add_subdirectory(tests/integration)
endif(BUILD_TESTS)

if (BUILD_EXAMPLES)
    add_subdirectory(examples/sampleGradient)
    add_subdirectory(examples/sampleModels)
endif(BUILD_EXAMPLES)

add_library(raytracer src/raytracer.cpp)
target_link_libraries(raytracer PUBLIC geometry utility physics)
target_include_directories(raytracer PUBLIC ./include)
