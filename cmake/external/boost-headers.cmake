cmake_minimum_required(VERSION 2.8.2)
project(boost-download NONE)

include(ExternalProject)
ExternalProject_Add(
        boost
        URL https://dl.bintray.com/boostorg/release/1.66.0/source/boost_1_66_0.tar.gz
        SOURCE_DIR "${EXTERNAL_BINARY_DIR}/boost-src"
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        TEST_COMMAND ""
)