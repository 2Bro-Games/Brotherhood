cmake_minimum_required(VERSION 3.5.0)
project(Brotherhood)

# set cxx standard
set(CMAKE_CXX_STANDARD 20)

# some definitions
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_compile_definitions(DEBUG=1)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
    add_compile_definitions(DEBUG=0)
endif()

# add submodules
add_subdirectory(engine)
add_subdirectory(editor)