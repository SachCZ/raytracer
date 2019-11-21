set(MESSAGE_QUIET ON)
if (NOT TARGET jsoncpp_lib)
    set(JSONCPP_WITH_TESTS OFF)
    set(JSONCPP_WITH_POST_BUILD_UNITTEST OFF)
    set(JSONCPP_WITH_WARNING_AS_ERROR OFF)
    set(JSONCPP_WITH_STRICT_ISO OFF)
    set(JSONCPP_WITH_PKGCONFIG_SUPPORT OFF)
    set(JSONCPP_WITH_CMAKE_PACKAGE OFF)
    set(BUILD_SHARED_LIBS OFF)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../../external/jsoncpp ${CMAKE_CURRENT_BINARY_DIR}/../../external/jsoncpp EXCLUDE_FROM_ALL)
endif ()

unset(MESSAGE_QUIET)
message(STATUS "Building jsoncpp finished")

message(STATUS ${CMAKE_CURRENT_SOURCE_DIR}/../../external/jsoncpp/include)


add_library(utility AdjacencyList.cpp JsonFormatter.cpp)
target_include_directories(utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include/raytracer/utility ${CMAKE_CURRENT_SOURCE_DIR}/../../external/jsoncpp/include)
target_compile_options(utility PRIVATE -Werror -Wall -Wextra)
