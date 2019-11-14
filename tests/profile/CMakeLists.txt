add_executable(Profile profile.cpp)
target_link_libraries(Profile PRIVATE raytracer)
configure_file(mesh.vtk ${CMAKE_CURRENT_BINARY_DIR}/mesh.vtk COPYONLY)
