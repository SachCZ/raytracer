message(STATUS "Building gtest")

set(MESSAGE_QUIET ON)
if (NOT TARGET gtest)
    set(INSTALL_GTEST OFF)
    add_subdirectory(../external/gtest ${CMAKE_BINARY_DIR}/external/gtest EXCLUDE_FROM_ALL)
endif ()

unset(MESSAGE_QUIET)
message(STATUS "Building gtest finished")

add_executable(TestRunner testRunner.cpp geometry/rayTest.cpp geometry/pointTest.cpp geometry/vectorTest.cpp geometry/edgeTest.cpp geometry/triangleTest.cpp utility/adjacencyListTest.cpp geometry/meshTest.cpp utility/jsonFormatterTest.cpp)
target_link_libraries(TestRunner gtest gtest gmock RayTracer)
configure_file(./geometry/mesh.stl ${CMAKE_CURRENT_BINARY_DIR}/geometry/mesh.stl COPYONLY)
