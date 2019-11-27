add_executable(TestRunner testRunner.cpp geometry/rayTest.cpp geometry/pointTest.cpp geometry/vectorTest.cpp geometry/edgeTest.cpp geometry/quadrilateralTest.cpp utility/adjacencyListTest.cpp geometry/meshTest.cpp geometry/meshFunctionTest.cpp physics/laserRayTest.cpp physics/functionsTest.cpp physics/laserTest.cpp utility/freeFunctionsTest.cpp)
target_link_libraries(TestRunner PRIVATE gtest gtest gmock raytracer)
configure_file(geometry/mesh.vtk ${CMAKE_CURRENT_BINARY_DIR}/geometry/mesh.vtk COPYONLY)
