include(GoogleTest)
enable_testing()

add_library(tests_support support/mocks.cpp support/matchers.cpp)
target_include_directories(tests_support PUBLIC ${CMAKE_SOURCE_DIR}/tests/support)
target_link_libraries(tests_support PUBLIC gtest gmock)
target_link_libraries(tests_support PRIVATE gtest geometry)

add_executable(unit_tests
        unit/unit_test_runner.cpp
        unit/geometry/point_test.cpp
        unit/geometry/vector_test.cpp
        unit/geometry/mesh_test.cpp
        unit/geometry/mesh_function_test.cpp
        unit/geometry/intersection_test.cpp
        unit/geometry/element_test.cpp
        unit/physics/models_test.cpp
        unit/physics/laser_test.cpp
        unit/physics/propagation_test.cpp
        unit/physics/gradient_test.cpp
        unit/utility/numeric_test.cpp
        unit/utility/qr_decomposition_test.cpp
        unit/physics/absorption_test.cpp)
target_link_libraries(unit_tests PRIVATE geometry physics utility tests_support)
gtest_add_tests(TARGET unit_tests)

add_executable(integration_tests
        integration/integration_test_runner.cpp
        integration/quadratic_density.cpp
        integration/mfem_gradient.cpp
        integration/hous_gradient.cpp)
target_link_libraries(integration_tests PRIVATE raytracer tests_support)
gtest_add_tests(TARGET integration_tests)