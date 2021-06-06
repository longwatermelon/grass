#include "scrollbar.h"
#include "common.h"
#include <iostream>


gui::Scrollbar::Scrollbar(SDL_Rect rect, int min_bar_bound, int max_bar_bound, int total_size, SDL_Color bg_color, SDL_Color bar_color)
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


void gui::Scrollbar::render(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    SDL_SetRenderDrawColor(rend, m_bar_color.r, m_bar_color.g, m_bar_color.b, 255);
    SDL_RenderFillRect(rend, &m_bar_rect);
}


void gui::Scrollbar::move(int x, int y)
{
    m_rect.x += x;
    m_bar_rect.x += x;

    m_rect.y += y;
    m_bar_rect.y += y;
}


void gui::Scrollbar::resize(int window_h)
{
    m_rect.h = window_h - m_rect.y;
}


void gui::Scrollbar::set_bounds(int min_bar_bound, int max_bar_bound, int total_size)
{
    m_total_bar_height = total_size;
    
    m_bar_rect.y = y_to_bar_pos(min_bar_bound);
    m_bar_rect.h = y_to_bar_pos(max_bar_bound - min_bar_bound + 1) - 1;

    m_bar_rect.y += m_rect.y;
}


int gui::Scrollbar::y_to_bar_pos(int y)
{
    if (m_total_bar_height == 0)
        return 0;

    int result = (int)((float)y * (float)((float)m_rect.h / (float)m_total_bar_height));

    return result;
}


void gui::Scrollbar::check_clicked(int mx, int my)
{
    if (common::within_rect(m_bar_rect, mx, my))
    {
        m_down = true;
        m_bar_and_mouse_diff = my - m_bar_rect.y;
    }
}


void gui::Scrollbar::mouse_up()
{
    m_down = false;
    m_bar_and_mouse_diff = 0;
}


void gui::Scrollbar::move_with_cursor(int my)
{
    m_bar_rect.y = std::max(std::min(m_rect.y + (int)((float)((my - m_bar_and_mouse_diff) - m_rect.y) / (m_rect.h / m_total_bar_height)) * (m_rect.h / m_total_bar_height), m_rect.y + m_rect.h - m_bar_rect.h), m_rect.y);
}


int gui::Scrollbar::min_position()
{
    return ((float)(m_bar_rect.y - m_rect.y) / (float)(m_rect.h)) * (m_total_bar_height + 1);
}