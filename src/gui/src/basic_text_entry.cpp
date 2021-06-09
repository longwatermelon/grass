#include "basic_text_entry.h"


gui::BasicTextEntry::BasicTextEntry(SDL_Rect rect, const Cursor& cursor, std::unique_ptr<Text> text, SDL_Color bg_color)
    : m_rect(rect), m_cursor(cursor), m_text(std::move(text)), m_bg_color(bg_color) {}


void gui::BasicTextEntry::render(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    m_text->render();
}