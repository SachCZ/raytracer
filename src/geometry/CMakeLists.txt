add_library(geometry Ray.cpp Point.cpp Vector.cpp Edge.cpp Triangle.cpp Mesh.cpp)
target_include_directories(geometry PUBLIC ../../include/raytracer/geometry ../../include)
target_link_libraries(geometry PUBLIC utility)
target_compile_options(geometry PRIVATE -Werror -Wall -Wextra)
