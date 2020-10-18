add_library(physics
        collisional_frequency.cpp
        laser.cpp
        propagation.cpp
        gradient.cpp
        refraction.cpp
        termination.cpp
        absorption.cpp
        )
target_include_directories(physics PUBLIC ../../include/raytracer/physics)
target_link_libraries(physics PRIVATE utility jsoncpp msgpack)
target_link_libraries(physics PUBLIC geometry)
target_compile_options(physics PRIVATE $<$<CONFIG:DEBUG>:-Werror -Wall -Wextra>)
target_compile_options(physics PRIVATE $<$<CONFIG:RELEASE>:-O3>)
target_compile_options(physics PRIVATE $<$<CONFIG:COVERAGE>:--coverage -g -O0>)
target_link_options(physics PUBLIC $<$<CONFIG:COVERAGE>:--coverage>)
