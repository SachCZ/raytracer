add_executable(Resonance Resonance.cpp)
target_link_libraries(Resonance PRIVATE raytracer)
target_compile_options(Resonance PRIVATE $<$<CONFIG:RELEASE>:-O3>)

