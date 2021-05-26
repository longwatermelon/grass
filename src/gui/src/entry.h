#pragma once
#include "text.h"
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>


namespace gui
{
    class TextEntry
    {
    public:
        TextEntry(SDL_Rect rect, int line_limit, const Text& text, bool show_line_numbers = false, SDL_Color bg_col = { 255, 255, 255 });

    public:
        void render(SDL_Renderer* rend);
        void add_char(char c);
        void remove_char(int count);

        bool check_clicked(int mx, int my);

        std::vector<std::string> get_visible_content();
        void move_cursor(int x, int y);
        SDL_Point real_to_char_pos(SDL_Point pos);

        void move_bounds(int x, int y);
        void reset_bounds_x();
        void reset_bounds_y();

        void draw_cursor(SDL_Renderer* rend);

    private:
        SDL_Rect m_rect;
        Text m_text;

        int m_line_limit{ -1 };
        bool m_show_line_numbers;
        SDL_Color m_background_color;

        SDL_Point m_cursor_pos;
        SDL_Point m_real_cursor_pos;

        // indexes of strings not pixel coordinates
        SDL_Point m_min_visible, m_max_visible;
        std::vector<std::string> m_visible_content;
    };
}