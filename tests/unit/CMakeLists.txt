message(STATUS "Building gtest")

set(MESSAGE_QUIET ON)
if (NOT TARGET gtest)
    set(INSTALL_GTEST OFF)
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/gtest ${CMAKE_BINARY_DIR}/external/gtest EXCLUDE_FROM_ALL)
endif ()

unset(MESSAGE_QUIET)
message(STATUS "Building gtest finished")

add_executable(TestRunner testRunner.cpp geometry/rayTest.cpp geometry/pointTest.cpp geometry/vectorTest.cpp geometry/edgeTest.cpp geometry/quadrilateralTest.cpp utility/adjacencyListTest.cpp geometry/meshTest.cpp utility/jsonFormatterTest.cpp geometry/meshFunctionTest.cpp physics/laserRayTest.cpp physics/functionsTest.cpp physics/laserTest.cpp utility/freeFunctionsTest.cpp)
target_link_libraries(TestRunner PRIVATE gtest gtest gmock raytracer utility)
configure_file(geometry/mesh.vtk ${CMAKE_CURRENT_BINARY_DIR}/geometry/mesh.vtk COPYONLY)
