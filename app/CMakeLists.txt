cmake_minimum_required(VERSION 3.20)

add_executable(swipesonic_app
  main.cpp
)

# Link raylib from the top-level FetchContent
target_link_libraries(swipesonic_app PRIVATE raylib)

# Nice warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
  target_compile_options(swipesonic_app PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wno-unused-parameter)
endif()
