cmake_minimum_required(VERSION 3.19.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(grass)

add_subdirectory(src/grass)
add_subdirectory(src/gui)

set_property(TARGET grass PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
