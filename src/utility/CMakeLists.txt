add_library(utility
        numeric.cpp
        qr_decomposition.cpp)
target_include_directories(utility PUBLIC ../../include/raytracer/utility)
target_compile_options(utility PRIVATE $<$<CONFIG:DEBUG>:-Werror -Wall -Wextra>)
target_compile_options(utility PRIVATE $<$<CONFIG:RELEASE>:-O3>)
target_compile_options(utility PRIVATE $<$<CONFIG:COVERAGE>:--coverage -g -O0>)
target_link_options(utility PUBLIC $<$<CONFIG:COVERAGE>:--coverage>)