cmake_minimum_required(VERSION 3.5.0)
project(Extern)

add_subdirectory(glfw)
add_subdirectory(glm)

add_library(Extern INTERFACE)
target_sources(Extern INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc/vk_mem_alloc.cpp")
target_include_directories(Extern INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vk_mem_alloc")