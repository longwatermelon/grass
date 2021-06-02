project(gui)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)

add_library(gui
    src/button.h
    src/button.cpp
    src/common.h
    src/common.cpp
    src/entry.h
    src/entry.cpp
    src/text.h
    src/text.cpp
    src/file_tree.h
    src/file_tree.cpp
)

target_link_libraries(gui PUBLIC
    SDL2::SDL2 SDL2::SDL2main
    SDL2::SDL2_ttf
)

target_include_directories(grass PRIVATE src)