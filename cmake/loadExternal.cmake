set(EXTERNAL_BINARY_DIR ${CMAKE_BINARY_DIR}/external)

file(GLOB files "cmake/external/*.cmake")

foreach(file ${files})
    get_filename_component(filename ${file} NAME_WE)
    configure_file(cmake/external/${filename}.cmake ${EXTERNAL_BINARY_DIR}/${filename}-download/CMakeLists.txt)
    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" . WORKING_DIRECTORY "${EXTERNAL_BINARY_DIR}/${filename}-download")
    execute_process(COMMAND "${CMAKE_COMMAND}" --build . WORKING_DIRECTORY "${EXTERNAL_BINARY_DIR}/${filename}-download")
endforeach()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory("${EXTERNAL_BINARY_DIR}/gtest-src" "${EXTERNAL_BINARY_DIR}/gtest-build")

add_library(jsoncpp STATIC IMPORTED)
set_target_properties(
        jsoncpp PROPERTIES
        "IMPORTED_LOCATION" "${EXTERNAL_BINARY_DIR}/jsoncpp-build/src/lib_json/libjsoncpp.a"
        "INTERFACE_INCLUDE_DIRECTORIES" "${EXTERNAL_BINARY_DIR}/jsoncpp-src/include/"
)

add_library(mfem STATIC IMPORTED)
set_target_properties(
        mfem PROPERTIES
        "IMPORTED_LOCATION" "${EXTERNAL_BINARY_DIR}/mfem-build/libmfem.a"
        "INTERFACE_INCLUDE_DIRECTORIES" "${EXTERNAL_BINARY_DIR}/mfem-build/"
)