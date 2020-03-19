cmake_minimum_required(VERSION 2.8.2)
project(mfem-download NONE)

include(ExternalProject)
ExternalProject_Add(
        googletest
        GIT_REPOSITORY https://github.com/mfem/mfem
        GIT_TAG 3a5022f1bb
        SOURCE_DIR "${EXTERNAL_BINARY_DIR}/mfem-src"
        BINARY_DIR "${EXTERNAL_BINARY_DIR}/mfem-build"
        INSTALL_COMMAND ""
        TEST_COMMAND ""
)