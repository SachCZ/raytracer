add_executable(Integration integration.cpp tracing.cpp)
target_link_libraries(Integration PRIVATE gtest gtest gmock raytracer)
configure_file(mesh.vtk ${CMAKE_CURRENT_BINARY_DIR}/mesh.vtk COPYONLY)
