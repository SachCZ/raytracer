cmake_minimum_required(VERSION 2.8.2)
project(msgpack-download NONE)

include(ExternalProject)
ExternalProject_Add(
        msgpack
        GIT_REPOSITORY https://github.com/msgpack/msgpack-c.git
        GIT_TAG cpp_master
        SOURCE_DIR "${EXTERNAL_BINARY_DIR}/msgpack-src"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
)