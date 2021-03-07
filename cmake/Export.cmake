install(TARGETS raytracer geometry utility physics msgpack jsoncpp_static EXPORT raytracerTargets
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )

install(DIRECTORY include/ DESTINATION include/raytracer FILES_MATCHING PATTERN "*.h")

install(TARGETS raytracer EXPORT )

install(EXPORT raytracerTargets
        FILE raytracerTargets.cmake
        NAMESPACE raytracer::
        DESTINATION lib/cmake/raytracer
        )

include(CMakePackageConfigHelpers)
write_basic_package_version_file("raytracerConfigVersion.cmake"
        VERSION 0.1
        COMPATIBILITY SameMajorVersion
        )
install(FILES "cmake/raytracerConfig.cmake" "${CMAKE_BINARY_DIR}/raytracerConfigVersion.cmake"
        DESTINATION lib/cmake/raytracer)