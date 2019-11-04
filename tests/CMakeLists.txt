message(STATUS "Building gtest")

set(MESSAGE_QUIET ON)
if (NOT TARGET gtest)
    set(INSTALL_GTEST OFF)
    add_subdirectory(../external/gtest ${CMAKE_BINARY_DIR}/external/gtest EXCLUDE_FROM_ALL)
endif ()

unset(MESSAGE_QUIET)
message(STATUS "Building gtest finished")

add_executable(TestRunner testRunner.cpp geometry/rayTest.cpp)
target_link_libraries(TestRunner gtest gtest gmock RayTracer)