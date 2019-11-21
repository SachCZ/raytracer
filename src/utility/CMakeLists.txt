add_library(utility AdjacencyList.cpp JsonFormatter.cpp ../../include/raytracer/utility/JsonFormatter.h ../../include/raytracer/utility/FreeFunctions.h)
target_include_directories(utility PUBLIC ../../include/raytracer/utility)
target_compile_options(utility PRIVATE -Werror -Wall -Wextra)
