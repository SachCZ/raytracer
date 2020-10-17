add_library(geometry
        mesh.cpp
        intersection.cpp
        mesh_function.cpp
        geometry_primitives.cpp
        )
target_include_directories(geometry PUBLIC ../../include/raytracer/geometry)
target_link_libraries(geometry PUBLIC mfem)
target_compile_options(geometry PRIVATE $<$<CONFIG:DEBUG>:-Werror -Wall -Wextra>)
target_compile_options(geometry PRIVATE $<$<CONFIG:RELEASE>:-O3>)
