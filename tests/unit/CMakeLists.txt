add_executable(TestRunner
        testRunner.cpp
        geometry/point_test.cpp
        geometry/vector_test.cpp
        geometry/mesh_test.cpp
        geometry/mesh_function_test.cpp
        geometry/intersection_test.cpp
        geometry/element_test.cpp
        physics/models_test.cpp
        physics/laser_test.cpp
        physics/propagation_test.cpp
        physics/gradient_test.cpp
        utility/numeric_test.cpp
        utility/qr_decomposition_test.cpp
        physics/absorption_test.cpp
        mocks.cpp mocks.h)
target_link_libraries(TestRunner PRIVATE gtest gtest gmock geometry physics utility)
configure_file(geometry/mesh.vtk ${CMAKE_CURRENT_BINARY_DIR}/geometry/mesh.vtk COPYONLY)
