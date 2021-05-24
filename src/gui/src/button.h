#pragma once
#include <string>
#include <functional>
#include <SDL.h>
#include <SDL_ttf.h>


namespace gui
{
    class Button
    {
    public:
        Button(const char* text, SDL_Rect rect, SDL_Color color, const std::function<void()>& func);

    public:
        void render(SDL_Renderer* rend, TTF_Font* font);

        void check_clicked(int mx, int my);

        void set_down(bool b) { m_down = b; }

    private:
        std::string m_text;

        SDL_Rect m_rect;
        SDL_Color m_color;

        std::function<void()> m_function;
        bool m_down{ false };
    };
}