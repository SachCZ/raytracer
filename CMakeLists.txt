cmake_minimum_required(VERSION 3.1)
project(RayTracer)

#setup external libs-----------------------------
include(cmake/loadExternal.cmake)

set(CMAKE_CXX_STANDARD 14)

add_subdirectory(src/geometry)
add_subdirectory(src/utility)
add_subdirectory(src/physics)

option(BUILD_TESTS "Build unit tests" OFF)

if(BUILD_TESTS)
    add_subdirectory(tests/unit)
    add_subdirectory(tests/integration)
endif(BUILD_TESTS)

if (BUILD_EXAMPLES)
    add_subdirectory(examples/sampleGradient)
    add_subdirectory(examples/sampleModels)
    add_subdirectory(examples/traceBremsstrahlung)
    add_subdirectory(examples/traceResonance)
endif(BUILD_EXAMPLES)

add_library(raytracer src/raytracer.cpp src/physics/Absorption.cpp include/raytracer/physics/Absorption.h)
target_link_libraries(raytracer PUBLIC geometry utility physics)
target_include_directories(raytracer PUBLIC ./include)
