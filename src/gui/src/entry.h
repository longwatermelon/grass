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

    private:
        SDL_Rect m_rect;
        Text m_text;

        int m_line_limit{ -1 };
        bool m_show_line_numbers;
        SDL_Color m_background_color;
    };
}