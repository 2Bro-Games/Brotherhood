cmake_minimum_required(VERSION 3.5.0)
project(Engine)

file(GLOB_RECURSE ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/externs/glfw)

add_library(Engine STATIC ${ENGINE_SOURCES})

target_include_directories(Engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source)
target_link_libraries(Engine PUBLIC glfw)