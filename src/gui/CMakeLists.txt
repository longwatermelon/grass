project(gui)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)
find_package(sdl2-image CONFIG REQUIRED)

add_library(gui
    button.h
    button.cpp
    common.h
    common.cpp
    gui_string.h
    gui_string.cpp
    file_tree.h
    file_tree.cpp
    cursor.h
    cursor.cpp
    text_entry.h
    text_entry.cpp
    explorer.h
    explorer.cpp
    scrollbar.h
    scrollbar.cpp
    text.h
    text.cpp
    menu.h
    menu.cpp
    basic_text_entry.h
    basic_text_entry.cpp
    tab.h
    tab.cpp
)

target_link_libraries(gui PUBLIC
    SDL2::SDL2 SDL2::SDL2main
    SDL2::SDL2_ttf
    SDL2::SDL2_image
)

target_include_directories(grass PRIVATE src)
