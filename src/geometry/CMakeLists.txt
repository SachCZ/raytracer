add_library(geometry Ray.cpp include/geometry/Ray.h Point.cpp include/geometry/Point.h Vector.cpp include/geometry/Vector.h Edge.cpp include/geometry/Edge.h include/geometry.h Triangle.cpp include/geometry/Triangle.h Mesh.cpp include/geometry/Mesh.h)
target_include_directories(geometry PUBLIC ./include)
target_compile_options(geometry PRIVATE -Werror -Wall -Wextra)
