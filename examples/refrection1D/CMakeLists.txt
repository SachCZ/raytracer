add_executable(Refraction1D refraction1D.cpp)
target_link_libraries(Refraction1D PRIVATE raytracer yamlcpp)
target_compile_options(Refraction1D PRIVATE $<$<CONFIG:RELEASE>:-O3>)

