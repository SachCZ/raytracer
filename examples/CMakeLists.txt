add_executable(TraceLine traceLine.cpp)
target_link_libraries(TraceLine RayTracer)
configure_file(./mesh.stl ${CMAKE_CURRENT_BINARY_DIR}/mesh.stl COPYONLY)
