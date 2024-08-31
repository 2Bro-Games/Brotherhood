cmake_minimum_required(VERSION 3.5.0)
project(Window)

# Путь к исходникам GLFW
add_subdirectory(external/glfw)

# Создаем библиотеку из window.cpp
add_library(window STATIC window.cpp)

# Указываем путь к заголовочным файлам для GLFW
target_include_directories(window PUBLIC ${CMAKE_SOURCE_DIR}/external/glfw/include)

# Указываем, что window зависит от библиотеки glfw
target_link_libraries(window PRIVATE glfw)

# Указываем, что заголовочные файлы находятся в текущей директории
target_include_directories(window PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})