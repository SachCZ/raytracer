cmake_minimum_required(VERSION 3.11)
project(RayTracer)

set(CMAKE_CXX_STANDARD 14)
set(BUILD_SHARED_LIBS OFF)

option(RAYTRACER_BUILD_TESTS OFF)
option(RAYTRACER_COMPILE_COVERAGE OFF)
option(RAYTRACER_BUILD_SAMPLES OFF)

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
    set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE STRING "Choose the type of build." FORCE)
    # Set the possible values of build type for cmake-gui
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release")
endif()


# Setup external targets
include(cmake/External.cmake)
#TODO possibly an anti-pattern, look into this


#Explicit is better than implicit
if(MSVC)
    add_compile_options("$<$<CONFIG:DEBUG>:/MDd;/Zi;/Ob0;/Od;/RTC1;/W4;/WX>")
    add_compile_options("$<$<CONFIG:RELEASE>:/MD;/O2;/Ob2;/DNDEBUG>")
else()
    add_compile_options("$<$<CONFIG:DEBUG>:-g;-Wall;-Wextra;-pedantic;-Werror>")
    add_compile_options("$<$<CONFIG:RELEASE>:-O3;-DNDEBUG>")
endif()
if (RAYTRACER_COMPILE_COVERAGE)
    add_compile_options(--coverage)
    add_link_options(--coverage)
endif()

add_subdirectory(src/geometry)
add_subdirectory(src/utility)
add_subdirectory(src/physics)

if (${RAYTRACER_BUILD_TESTS})
    add_subdirectory(tests)
endif()

if (${RAYTRACER_BUILD_PROFILE})
    add_subdirectory(profile)
endif()

if (${RAYTRACER_BUILD_SAMPLES})
    add_subdirectory(samples)
endif()

add_library(raytracer src/raytracer.cpp)
target_link_libraries(raytracer PUBLIC geometry utility physics)
target_include_directories(raytracer PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include/raytracer>)

include(cmake/Export.cmake)