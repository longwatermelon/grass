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

        /// <summary>
        /// If (mx, my) is inside of m_rect, it will call m_function.
        /// </summary>
        /// <param name="mx">Mouse x pos</param>
        /// <param name="my">Mouse y pos</param>
        void check_clicked(int mx, int my);

        /// <summary>
        /// If (mx, my) is inside of m_rect, m_hover will be set to true and the button will glow.
        /// </summary>
        /// <param name="mx">Mouse x pos</param>
        /// <param name="my">Mouse y pos</param>
        void check_hover(int mx, int my);

        void set_down(bool b) { m_down = b; }

    private:
        Text m_text;

        SDL_Rect m_rect;
        SDL_Color m_color;

        std::function<void(void)> m_function;
        bool m_down{ false };
        bool m_hover{ false };
    };
}