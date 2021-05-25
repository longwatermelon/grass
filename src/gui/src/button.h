#pragma once
#include "text.h"
#include <string>
#include <functional>
#include <SDL.h>
#include <SDL_ttf.h>


namespace gui
{
    class Button
    {
    public:
        Button(const Text& text, SDL_Rect rect, SDL_Color color, const std::function<void()>& func);

    public:
        void render(SDL_Renderer* rend);

        void check_clicked(int mx, int my);
        void check_hover(int mx, int my);

        void set_down(bool b) { m_down = b; }

    private:
        Text m_text;

        SDL_Rect m_rect;
        SDL_Color m_color;

        std::function<void()> m_function;
        bool m_down{ false };
        bool m_hover{ false };
    };
}