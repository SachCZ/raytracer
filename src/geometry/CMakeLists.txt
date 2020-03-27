add_library(geometry
        Ray.cpp
        Point.cpp
        Vector.cpp
        Mesh.cpp
        Intersection.cpp
        Face.cpp
        Element.cpp
        MeshFunction.cpp
        Matrix2D.cpp ../../include/raytracer/geometry/Matrix2D.h)
target_include_directories(geometry PUBLIC ../../include/raytracer/geometry)
target_link_libraries(geometry PUBLIC utility mfem)
target_compile_options(geometry PRIVATE $<$<CONFIG:DEBUG>:-Werror -Wall -Wextra>)
target_compile_options(geometry PRIVATE $<$<CONFIG:RELEASE>:-O3>)
