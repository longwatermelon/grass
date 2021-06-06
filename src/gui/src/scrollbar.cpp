#include "scrollbar.h"

Scrollbar::Scrollbar(SDL_Rect rect, int min_bar_bound, int max_bar_bound, int total_size, SDL_Color bg_color, SDL_Color bar_color)
    : m_rect(rect), m_bg_color(bg_color), m_bar_color(bar_color), m_total_bar_height(total_size)
{
    m_bar_rect = {
        m_rect.x,
        y_to_bar_pos(min_bar_bound) + m_rect.y,
        m_rect.w,
        y_to_bar_pos(max_bar_bound)
    };

    set_bounds(min_bar_bound, max_bar_bound, total_size);
}


void Scrollbar::render(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    SDL_SetRenderDrawColor(rend, m_bar_color.r, m_bar_color.g, m_bar_color.b, 255);
    SDL_RenderFillRect(rend, &m_bar_rect);
}


void Scrollbar::move(int x, int y)
{
    m_rect.x += x;
    m_bar_rect.x += x;

    m_rect.y += y;
    m_bar_rect.y += y;
}


void Scrollbar::resize(int window_h)
{
    m_rect.h = window_h;
}


void Scrollbar::set_bounds(int min_bar_bound, int max_bar_bound, int total_size)
{
    m_total_bar_height = total_size;
    
    m_bar_rect.y = y_to_bar_pos(min_bar_bound);
    m_bar_rect.h = y_to_bar_pos(max_bar_bound - min_bar_bound + 1);

    m_bar_rect.y += m_rect.y;
}


int Scrollbar::y_to_bar_pos(int y)
{
    if (m_total_bar_height == 0)
        return 0;

    int result = (int)((float)y * (float)((float)m_rect.h / (float)m_total_bar_height));

    return result;
}