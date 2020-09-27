cmake_minimum_required(VERSION 2.8.2)
project(jsoncpp-download NONE)

include(ExternalProject)
ExternalProject_Add(
        jsoncpp
        GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp
        GIT_TAG master
        SOURCE_DIR "${EXTERNAL_BINARY_DIR}/jsoncpp-src"
        BINARY_DIR "${EXTERNAL_BINARY_DIR}/jsoncpp-build"
        INSTALL_COMMAND ""
        TEST_COMMAND ""
)