add_library(physics
        LaserRay.cpp
        Laser.cpp
        ../../include/raytracer/physics/Laser.h
        Propagation.cpp
        ../../include/raytracer/physics/Propagation.h
        Gradient.cpp
        ../../include/raytracer/physics/Gradient.h
        Refraction.cpp
        ../../include/raytracer/physics/Refraction.h
        Termination.cpp
        ../../include/raytracer/physics/Termination.h
        )
target_include_directories(physics PUBLIC ../../include/raytracer/physics)
target_link_libraries(physics PUBLIC geometry utility jsoncpp)
target_compile_options(physics PRIVATE $<$<CONFIG:DEBUG>:-Werror -Wall -Wextra>)
target_compile_options(physics PRIVATE $<$<CONFIG:RELEASE>:-O3>)
