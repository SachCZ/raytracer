include(FetchContent)

Set(FETCHCONTENT_QUIET FALSE)

if (${RAYTRACER_BUILD_TESTS})
    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        release-1.8.0
            GIT_PROGRESS TRUE
    )

    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
        FetchContent_Populate(googletest)
        add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
    endif()
endif()

FetchContent_Declare(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
        GIT_TAG        1.9.4
        GIT_PROGRESS TRUE
)
FetchContent_GetProperties(jsoncpp)
if(NOT jsoncpp_POPULATED)
    FetchContent_Populate(jsoncpp)
    set(JSONCPP_WITH_TESTS OFF)
    add_subdirectory(${jsoncpp_SOURCE_DIR} ${jsoncpp_BINARY_DIR})
endif()

FetchContent_Declare(
        msgpack
        GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
        GIT_TAG        cpp-3.3.0
        GIT_PROGRESS TRUE
)
FetchContent_GetProperties(msgpack)
if(NOT msgpack_POPULATED)
    FetchContent_Populate(msgpack)
    add_library(msgpack INTERFACE)
    set_property(TARGET msgpack APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
            $<BUILD_INTERFACE:${msgpack_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
            )
    set_property(TARGET msgpack APPEND PROPERTY "INTERFACE_LINK_LIBRARIES" Boost::boost)
endif()

find_package(MFEM)
set_target_properties(mfem PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${MFEM_INCLUDE_DIRS}")

find_package(Boost)