project(grass)

add_executable(grass
	src/main.cpp
	src/grass.h
	src/grass.cpp
	src/common.h
)

target_link_libraries(grass PRIVATE gui)