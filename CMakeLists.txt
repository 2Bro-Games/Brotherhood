cmake_minimum_required(VERSION 3.5.0)
project(Brotherhood)

set(CMAKE_CXX_STANDARD 20)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(______________DEBUG______________)
    add_compile_definitions(DEBUG)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    message(______________RELEASE______________)
    add_compile_definitions(NDEBUG)
endif()

add_subdirectory(extern)
add_subdirectory(engine)
add_subdirectory(editor)