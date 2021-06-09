project(gui)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

add_library(gui
    src/button.h
    src/button.cpp
    src/common.h
    src/common.cpp
    src/gui_string.h
    src/gui_string.cpp
    src/file_tree.h
    src/file_tree.cpp
    src/cursor.h
    src/cursor.cpp
    src/text_entry.h
    src/text_entry.cpp
    src/explorer.h
    src/explorer.cpp
    src/scrollbar.h
    src/scrollbar.cpp
    src/text.h
    src/text.cpp
    src/menu.h
    src/menu.cpp
    src/basic_text_entry.h
    src/basic_text_entry.cpp
)

target_link_libraries(gui PUBLIC
    SDL2::SDL2 SDL2::SDL2main
    SDL2::SDL2_ttf
    SDL2::SDL2_image
)

target_include_directories(grass PRIVATE src)
