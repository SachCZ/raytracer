add_executable(integration_tests integration.cpp tracing.cpp single.cpp initial.cpp)
target_link_libraries(integration_tests PRIVATE gtest gtest gmock raytracer)
