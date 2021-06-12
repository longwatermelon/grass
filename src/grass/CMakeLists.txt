project(grass)

add_executable(grass
    src/main.cpp
    src/grass.h
    src/grass.cpp
)

target_link_libraries(grass PRIVATE gui)

install(TARGETS grass DESTINATION bin)
# hacky solution but it works
install(DIRECTORY ${CMAKE_BINARY_DIR}/src/grass/Release/ DESTINATION bin FILES_MATCHING PATTERN "*.dll")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/res/ DESTINATION bin/res)