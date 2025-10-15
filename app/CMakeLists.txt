cmake_minimum_required(VERSION 3.20)

add_executable(swipesonic_app
  main.cpp
  http.mm
  spotify_bridge.hpp
)

# Link raylib from top-level FetchContent
target_link_libraries(swipesonic_app PRIVATE raylib)

# macOS frameworks used by NSURLSession + JSON
target_link_libraries(swipesonic_app PRIVATE "-framework Foundation" "-framework Security" "-framework CFNetwork")

# Asset dir define (handy later)
target_compile_definitions(swipesonic_app PRIVATE ASSET_DIR="${CMAKE_SOURCE_DIR}/data")

# Warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
  target_compile_options(swipesonic_app PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wno-unused-parameter)
endif()
