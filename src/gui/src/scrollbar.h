#pragma once
#include <SDL.h>


namespace gui
{
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

        // sets m_down to true if clicked
        void check_clicked(int mx, int my);
        void mouse_up();

        // follows the cursor y position if m_down is true, returns the amount of units the bar was moved by
        void move_with_cursor(int my);

        int min_position();


        SDL_Rect rect() { return m_rect; }
        SDL_Rect bar_rect() { return m_bar_rect; }

        bool down() { return m_down; }

        void hide() { m_hidden = true; }
        void show() { m_hidden = false; }

        bool hidden() { return m_hidden; }

    private:
        SDL_Rect m_rect;
        SDL_Rect m_bar_rect;

        int m_total_bar_height;

        SDL_Color m_bg_color, m_bar_color;

        bool m_down{ false };
        int m_bar_and_mouse_diff{ 0 };

        bool m_hidden{ false };
    };
}