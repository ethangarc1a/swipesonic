cmake_minimum_required(VERSION 3.20)
project(swipesonic LANGUAGES C CXX)

# ----- macOS target so it runs on your machine -----
set(CMAKE_OSX_DEPLOYMENT_TARGET "11.0" CACHE STRING "Minimum macOS version" FORCE)

# ----- Fix CMake 4.x compatibility (this is what your error is about) -----
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Put built executables in build/bin/
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Build static libs so the app is self-contained
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

include(FetchContent)

# Fetch raylib v5.0; the timestamp flag silences that warning you saw
FetchContent_Declare(
  raylib
  URL https://github.com/raysan5/raylib/archive/refs/tags/5.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)
FetchContent_MakeAvailable(raylib)

add_subdirectory(app)
