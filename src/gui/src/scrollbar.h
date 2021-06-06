#pragma once
#include <SDL.h>


class Scrollbar
{
public:
    Scrollbar(SDL_Rect rect, int min_bar_bound, int max_bar_bound, int total_size, SDL_Color bg_color, SDL_Color bar_color);

    void render(SDL_Renderer* rend);

    void move(int x, int y);

    void resize(int window_h);

    void set_bounds(int min_bar_bound, int max_bar_bound, int total_size);

    // converts a y position between 0 and m_total_bar_height to where it would display on the bar
    int y_to_bar_pos(int y);


    SDL_Rect rect() { return m_rect; }

private:
    SDL_Rect m_rect;
    SDL_Rect m_bar_rect;

    int m_total_bar_height;

    SDL_Color m_bg_color, m_bar_color;
};