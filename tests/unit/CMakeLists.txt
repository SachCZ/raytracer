add_executable(TestRunner
        testRunner.cpp
        geometry/rayTest.cpp
        geometry/pointTest.cpp
        geometry/vectorTest.cpp
        geometry/meshTest.cpp
        geometry/meshFunctionTest.cpp
        geometry/freeFunctionsTest.cpp
        physics/laserRayTest.cpp
        physics/laserTest.cpp
        physics/propagationTest.cpp
        physics/gradientTest.cpp
        utility/freeFunctionsTest.cpp
        physics/absorptionTest.cpp)
target_link_libraries(TestRunner PRIVATE gtest gtest gmock raytracer)
configure_file(geometry/mesh.vtk ${CMAKE_CURRENT_BINARY_DIR}/geometry/mesh.vtk COPYONLY)
