#include "entry.h"
#include "common.h"


gui::TextEntry::TextEntry(SDL_Rect rect, int line_limit, const Text& text, bool show_line_numbers, SDL_Color bg_col)
    : m_rect(rect), m_line_limit(line_limit), m_text(text), m_show_line_numbers(show_line_numbers), m_background_color(bg_col) {}


void gui::TextEntry::render(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, m_background_color.r, m_background_color.g, m_background_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    m_text.render(rend);
}


void gui::TextEntry::add_char(char c)
{
    m_text.append(c);
}


void gui::TextEntry::remove_char(int count)
{
    for (int i = 0; i < count; ++i)
    {
        m_text.pop_back();
    }
}


bool gui::TextEntry::check_clicked(int mx, int my)
{
    return common::within_rect(m_rect, mx, my);
}