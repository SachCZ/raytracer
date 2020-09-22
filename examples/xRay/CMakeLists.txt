add_executable(XRay xRay.cpp)
target_link_libraries(XRay PRIVATE raytracer)
target_compile_options(XRay PRIVATE $<$<CONFIG:RELEASE>:-O3>)

