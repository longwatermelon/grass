#include "button.h"
#include "common.h"
#include <iostream>


gui::Button::Button(SDL_Renderer* rend, const Text& text, SDL_Rect rect, SDL_Color color, const std::function<void()>& func)
    : m_text(text), m_rect(rect), m_color(color), m_function(func)
{
    m_tex = common::render_text(rend, m_text.font(), m_text.str().c_str(), m_text.color());
}


gui::Button::~Button()
{
    if (m_tex)
        SDL_DestroyTexture(m_tex);

    m_tex = nullptr;
}


void gui::Button::render(SDL_Renderer* rend)
{
    SDL_Color col = m_color;

    if (m_down)
    {
        col = {
            (Uint8)std::max(m_color.r - 20, 0),
            (Uint8)std::max(m_color.g - 20, 0),
            (Uint8)std::max(m_color.b - 20, 0)
        };
    }
    else if (m_hover)
    {
        col = {
            (Uint8)std::min(m_color.r + 20, 255),
            (Uint8)std::min(m_color.g + 20, 255),
            (Uint8)std::min(m_color.b + 20, 255)
        };
    }

    SDL_SetRenderDrawColor(rend, col.r, col.g, col.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    common::center_rendered_text(rend, m_tex, m_text.str().c_str(), m_rect, m_text.char_dim(), m_text.color());
}


bool gui::Button::check_clicked(int mx, int my)
{
    if (common::within_rect(m_rect, mx, my))
    {
        m_down = true;
        m_function();
        return true;
    }

    return false;
}


void gui::Button::check_hover(int mx, int my)
{
    m_hover = common::within_rect(m_rect, mx, my);
}