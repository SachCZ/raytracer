add_executable(Integration integration.cpp tracing.cpp single.cpp initial.cpp)
target_link_libraries(Integration PRIVATE gtest gtest gmock raytracer)
