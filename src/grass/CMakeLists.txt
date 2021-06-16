project(grass)

add_executable(grass
    src/main.cpp
    src/grass.h
    src/grass.cpp
    src/plugin_manager.h
    src/plugin_manager.cpp
)

target_link_libraries(grass PRIVATE gui plugin)

install(TARGETS grass DESTINATION bin)
# hacky solution but it works
install(DIRECTORY ${CMAKE_BINARY_DIR}/src/grass/Release/ DESTINATION bin FILES_MATCHING PATTERN "*.dll")
install(DIRECTORY ${CMAKE_SOURCE_DIR}/res/ DESTINATION bin/res)
