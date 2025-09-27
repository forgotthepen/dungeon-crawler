cmake_minimum_required(VERSION 3.16)
project(DungeonCrawler VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Option to enable Wayland support
option(USE_WAYLAND "Enable native Wayland support" OFF)

FetchContent_Declare(
  raylib
  GIT_REPOSITORY https://github.com/raysan5/raylib.git
  GIT_TAG 5.5
)

if (USE_WAYLAND)
  set(GLFW_BUILD_WAYLAND ON CACHE BOOL "" FORCE)
else()
  set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)
endif()

FetchContent_MakeAvailable(raylib)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  "src/*.cpp"
  "include/*.cpp"
  "include/*.hpp"
  "include/*.h"
)

add_executable(DungeonCrawler ${SRC_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
target_include_directories(${PROJECT_NAME} PRIVATE
  "include"
)