add_library(utility numeric.cpp)

target_include_directories(utility PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/internal/utility>
        $<INSTALL_INTERFACE:include/raytracer/internal/utility>)