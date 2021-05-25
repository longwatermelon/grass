#include "button.h"
#include "common.h"


gui::Button::Button(const char* text, SDL_Rect rect, SDL_Color color, const std::function<void()>& func)
    : m_text(text), m_rect(rect), m_color(color), m_function(func) {}


void gui::Button::render(SDL_Renderer* rend, TTF_Font* font)
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

    common::draw_centered_text(rend, font, m_text.c_str(), { m_rect.x, m_rect.y }, m_rect, { 10, 20 });
}


void gui::Button::check_clicked(int mx, int my)
{
    if (common::within_rect(m_rect, mx, my))
    {
        m_down = true;
        m_function();
    }
}


void gui::Button::check_hover(int mx, int my)
{
    m_hover = common::within_rect(m_rect, mx, my);
}