#pragma once
#include "common.h"
#include "cursor.h"
#include "text.h"
#include <string>
#include <vector>


namespace gui
{
    class BasicTextEntry
    {
    public:
        BasicTextEntry(SDL_Rect rect, const Cursor& cursor, std::unique_ptr<Text> text, SDL_Color bg_color);

        void render(SDL_Renderer* rend);

    private:
        SDL_Rect m_rect;
        Cursor m_cursor;
        std::unique_ptr<Text> m_text;

        SDL_Color m_bg_color;
    };
}