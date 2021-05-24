cmake_minimum_required(VERSION 3.19.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

project(grass)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)

add_executable(grass
	src/main.cpp
	src/grass.h
)

target_link_libraries(grass PRIVATE
	SDL2::SDL2 SDL2::SDL2main
	SDL2::SDL2_ttf
)

set_property(TARGET grass PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})
