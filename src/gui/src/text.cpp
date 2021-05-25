#include "text.h"
#include "common.h"


gui::Text::Text(TTF_Font* font, SDL_Point pos, const std::string& contents, SDL_Point char_dimensions, SDL_Color col)
    : m_font(font), m_contents(contents), m_char_dim(char_dimensions), m_color(col)
{
    m_rect = { pos.x, pos.y, char_dimensions.x * (int)contents.size(), char_dimensions.y };
}


void gui::Text::render(SDL_Renderer* rend)
{
    common::draw_text(rend, m_font, m_contents.c_str(), m_rect, m_color);
}


void gui::Text::render_centered(SDL_Renderer* rend, SDL_Rect rect)
{
    common::draw_centered_text(rend, m_font, m_contents.c_str(), { m_rect.x, m_rect.y }, rect, m_char_dim, m_color);
}


void gui::Text::move_to(int x, int y)
{
    m_rect.x = x;
    m_rect.y = y;
}