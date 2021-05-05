add_library(geometry
        mesh.cpp
        intersection.cpp
        mesh_function.cpp
        geometry_primitives.cpp
        )
target_include_directories(geometry PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/internal/geometry>
        $<INSTALL_INTERFACE:include/raytracer/internal/geometry>)
target_link_libraries(geometry PUBLIC utility)
target_link_libraries(geometry PUBLIC mfem)
