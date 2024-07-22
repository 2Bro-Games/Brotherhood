cmake_minimum_required(VERSION 3.5.0)
project(Editor)

# create a binary
add_executable(Editor Editor.cpp)

# include libraries
target_link_libraries(Editor PRIVATE Engine)
