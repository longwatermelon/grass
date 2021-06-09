#pragma once
#include "common.h"
#include "cursor.h"
#include "text.h"
#include <string>
#include <vector>


namespace gui
{
    // Used for stuff like prompts for file renaming
    class BasicTextEntry
    {
    public:
        BasicTextEntry(SDL_Rect rect, const Cursor& cursor, std::unique_ptr<Text> text, SDL_Color bg_color);

        void render(SDL_Renderer* rend);

        // add char where cursor is
        void add_char(char c);
        // remove char where cursor is
        void remove_char();

        bool check_clicked(int mx, int my);

        void set_cursor_visible(bool b) { m_render_cursor = b; }

    private:
        SDL_Rect m_rect;
        Cursor m_cursor;
        std::unique_ptr<Text> m_text;

        SDL_Color m_bg_color;

        bool m_render_cursor{ false };
    };
}