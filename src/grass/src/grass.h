#pragma once
#include "entry.h"


class Grass
{
public:
    Grass();
    ~Grass();

public:
    void mainloop();

private:
    SDL_Window* m_window;
    SDL_Renderer* m_rend;

    gui::TextEntry* m_selected_entry{ nullptr };
};