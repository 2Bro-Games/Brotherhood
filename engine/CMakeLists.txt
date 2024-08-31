cmake_minimum_required(VERSION 3.5.0)
project(Engine)

# Собираем исходники
file(GLOB_RECURSE ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp)

# Создаем статическую библиотеку
add_library(Engine STATIC ${ENGINE_SOURCES})

# Указываем директории для заголовочных файлов
target_include_directories(Engine PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/source
    ${CMAKE_SOURCE_DIR}/window
    ${CMAKE_SOURCE_DIR}/window/external/glfw/include
)

# Указываем, что библиотека Engine зависит от библиотеки window
target_link_libraries(Engine PRIVATE window)

# Создание исполняемого файла
add_executable(Brotherhood source/core/EntryPoint.cpp)

# Указываем зависимости для исполняемого файла
target_link_libraries(Brotherhood PRIVATE Engine glfw)
