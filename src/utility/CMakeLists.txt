add_library(utility
        FreeFunctions.cpp
        QRDecomposition.cpp)
target_include_directories(utility PUBLIC ${CMAKE_SOURCE_DIR}/include/raytracer/utility)
target_compile_options(utility PRIVATE $<$<CONFIG:DEBUG>:-Werror -Wall -Wextra>)
target_compile_options(utility PRIVATE $<$<CONFIG:RELEASE>:-O3>)
