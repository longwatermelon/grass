#include "cursor.h"


gui::Cursor::Cursor(SDL_Point pos, SDL_Color color, SDL_Point char_dim)
    : m_pos(pos), m_color(color), m_char_dim(char_dim) {}


void gui::Cursor::render(SDL_Renderer* rend, SDL_Point min_bounds)
{
    SDL_SetRenderDrawColor(rend, m_color.r, m_color.g, m_color.b, 255);

    SDL_Point display = display_pos(min_bounds);

    SDL_Rect cursor_rect = {
        display.x - 1,
        display.y,
        2,
        m_char_dim.y
    };

    SDL_RenderFillRect(rend, &cursor_rect);
}


SDL_Point gui::Cursor::pos()
{
    return m_pos;
}


SDL_Point gui::Cursor::display_pos(SDL_Point min_bounds)
{
    return { m_pos.x - min_bounds.x * m_char_dim.x, m_pos.y - min_bounds.y * m_char_dim.y };
}


SDL_Point gui::Cursor::char_pos(SDL_Rect rect)
{
    return { (m_pos.x - rect.x) / m_char_dim.x, (m_pos.y - rect.y) / m_char_dim.y };
}


SDL_Point gui::Cursor::display_char_pos(SDL_Rect rect, SDL_Point min_bounds)
{
    SDL_Point display = display_pos(min_bounds);
    return { (display.x - rect.x) / m_char_dim.x, (display.y - rect.y) / m_char_dim.y };
}


void gui::Cursor::move_pixels(int x, int y)
{
    m_pos.x += x;
    m_pos.y += y;
}


void gui::Cursor::move_characters(int x, int y)
{
    m_pos.x += x * m_char_dim.x;
    m_pos.y += y * m_char_dim.y;
}