cmake_minimum_required(VERSION 3.11)
project(RayTracer)

include(GoogleTest)
include(FetchContent)

set(CMAKE_CXX_STANDARD 14)

# Setup external targets
include(cmake/External.cmake)

if(MSVC)
    add_compile_options(/W4 /WX)
else()
    add_compile_options("$<$<CONFIG:DEBUG>:-Wall;-Wextra;-pedantic;-Werror>")
endif()

add_subdirectory(src/geometry)
add_subdirectory(src/utility)
add_subdirectory(src/physics)

enable_testing()
add_subdirectory(tests/unit)

add_library(raytracer src/raytracer.cpp)
target_link_libraries(raytracer PUBLIC geometry utility physics)
target_include_directories(raytracer PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/raytracer>)

message(${CMAKE_CURRENT_SOURCE_DIR}/include)

include(cmake/Export.cmake)