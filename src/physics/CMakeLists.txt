add_library(physics
        CollisionalFrequency.cpp
        Laser.cpp
        Propagation.cpp
        Gradient.cpp
        Refraction.cpp
        Termination.cpp
        Absorption.cpp
        )
target_include_directories(physics PUBLIC ../../include/raytracer/physics)
target_link_libraries(physics PUBLIC geometry utility jsoncpp msgpack)
target_compile_options(physics PRIVATE $<$<CONFIG:DEBUG>:-Werror -Wall -Wextra>)
target_compile_options(physics PRIVATE $<$<CONFIG:RELEASE>:-O3>)
