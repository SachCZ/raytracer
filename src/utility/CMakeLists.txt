add_library(utility AdjacencyList.cpp include/utility/AdjacencyList.h JsonFormatter.cpp include/utility/JsonFormatter.h)
target_include_directories(geometry PUBLIC ./include)
target_compile_options(geometry PRIVATE -Werror -Wall -Wextra)
