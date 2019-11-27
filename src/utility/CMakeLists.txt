add_library(utility AdjacencyList.cpp JsonFormatter.cpp)
target_link_libraries(utility PUBLIC jsoncpp)
target_include_directories(utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../../include/raytracer/utility ${CMAKE_CURRENT_SOURCE_DIR}/../../external/jsoncpp/include)
target_compile_options(utility PRIVATE -Werror -Wall -Wextra)
