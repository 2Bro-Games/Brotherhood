cmake_minimum_required(VERSION 3.5.0)
project(Engine)

# collect source files
file(GLOB_RECURSE ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

# create a static library
add_library(Engine STATIC ${ENGINE_SOURCES})

# include directories
target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)