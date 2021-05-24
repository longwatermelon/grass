project(gui)

find_package(SDL2 CONFIG REQUIRED)
find_package(sdl2-ttf CONFIG REQUIRED)

add_library(gui
    src/gui.h
    src/gui.cpp
)

target_link_libraries(grass PUBLIC
    SDL2::SDL2 SDL2::SDL2main
    SDL2::SDL2_ttf
)


target_include_directories(grass PUBLIC src)