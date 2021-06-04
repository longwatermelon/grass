#pragma once
#include <SDL.h>


namespace gui
{
    class Cursor
    {
    public:
        Cursor() = default;
        Cursor(SDL_Point pos, SDL_Color color, SDL_Point char_dim);

        void render(SDL_Renderer* rend, SDL_Point min_bounds);

        // returns cursor position measured in pixels
        SDL_Point pos();
        // returns where the cursor is to be drawn measured in pixels. min_bounds and max_bounds measured in characters
        SDL_Point display_pos(SDL_Point min_bounds);

        // returns the position of the cursor converted from pixels to characters
        SDL_Point char_pos(SDL_Rect rect);
        // returns the display position of the cursor converted from pixels to characters
        SDL_Point display_char_pos(SDL_Rect rect, SDL_Point min_bounds);

        // moves cursor by (x, y) pixels
        void move_pixels(int x, int y);
        // moves cursor by (x, y) characters
        void move_characters(int x, int y);


    private:
        // Real position of the cursor, measured in pixels
        SDL_Point m_pos;
        SDL_Color m_color;

        SDL_Point m_char_dim;
    };
}