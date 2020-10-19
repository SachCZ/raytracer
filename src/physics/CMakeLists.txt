add_library(physics
        collisional_frequency.cpp
        laser.cpp
        propagation.cpp
        gradient.cpp
        refraction.cpp
        termination.cpp
        absorption.cpp
        qr_decomposition.cpp
        )
target_include_directories(physics PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include/internal/physics>
        $<INSTALL_INTERFACE:include/raytracer/internal/physics>)
target_link_libraries(physics PRIVATE utility jsoncpp_static msgpack)
target_link_libraries(physics PUBLIC geometry)
