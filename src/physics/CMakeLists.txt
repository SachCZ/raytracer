add_library(physics
        LaserRay.cpp
        Laser.cpp
        Propagation.cpp
        Gradient.cpp
        Refraction.cpp
        Termination.cpp
        )
target_include_directories(physics PUBLIC ../../include/raytracer/physics)
target_link_libraries(physics PUBLIC geometry utility jsoncpp)
target_compile_options(physics PRIVATE $<$<CONFIG:DEBUG>:-Werror -Wall -Wextra>)
target_compile_options(physics PRIVATE $<$<CONFIG:RELEASE>:-O3>)
