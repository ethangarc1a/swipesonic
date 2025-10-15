cmake_minimum_required(VERSION 3.20)

add_executable(swipesonic_app
  main.cpp
  http.mm           # <— Objective-C++ bridge (HTTPS + JSON)
  spotify_bridge.hpp
)

# Link raylib from the top-level FetchContent
target_link_libraries(swipesonic_app PRIVATE raylib)

# macOS frameworks needed by NSURLSession/JSON
target_link_libraries(swipesonic_app PRIVATE "-framework Foundation" "-framework Security" "-framework CFNetwork")

# Warnings
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
  target_compile_options(swipesonic_app PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wno-unused-parameter)
endif()

# Pass an (optional) asset dir define for future use
target_compile_definitions(swipesonic_app PRIVATE ASSET_DIR="${CMAKE_SOURCE_DIR}/data")
