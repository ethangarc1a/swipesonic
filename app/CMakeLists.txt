cmake_minimum_required(VERSION 3.20)

add_executable(swipesonic_app
  main.cpp
  http.mm
  spotify_bridge.hpp
)

target_link_libraries(swipesonic_app PRIVATE raylib)

# macOS frameworks for NSURLSession + JSON
target_link_libraries(swipesonic_app PRIVATE "-framework Foundation" "-framework Security" "-framework CFNetwork")

# Pass asset dir define (handy later)
target_compile_definitions(swipesonic_app PRIVATE ASSET_DIR="${CMAKE_SOURCE_DIR}/data")

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
  target_compile_options(swipesonic_app PRIVATE -Wall -Wextra -Wshadow -Wconversion -Wno-unused-parameter)
endif()
