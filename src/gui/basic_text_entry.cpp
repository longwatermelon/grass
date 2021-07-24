#include "basic_text_entry.h"


gui::BasicTextEntry::BasicTextEntry(SDL_Rect rect, const Cursor& cursor, std::unique_ptr<Text> text, SDL_Color bg_color)
    : m_rect(rect), m_cursor(cursor), m_text(std::move(text)), m_bg_color(bg_color) {}


void gui::BasicTextEntry::render(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    m_text->render();

    if (m_render_cursor)
        m_cursor.render(rend, { 0, 0 });
}


void gui::BasicTextEntry::add_char(char c)
{
    std::string& text = m_text->text_ref();

    if ((m_cursor.char_pos(m_rect).x + 1) * m_text->font_ref().char_dim().x + m_rect.x < m_rect.x + m_rect.w)
    {
        text.insert(text.begin() + m_cursor.char_pos(m_rect).x, c);
        m_cursor.move_characters(1, 0);

        m_text->rerender_texture();
    }
}


void gui::BasicTextEntry::remove_char()
{
    std::string& text = m_text->text_ref();

    if ((m_cursor.char_pos(m_rect).x - 1) * m_text->font_ref().char_dim().x + m_rect.x >= m_rect.x)
    {
        text.erase(text.begin() + m_cursor.char_pos(m_rect).x - 1);
        m_cursor.move_characters(-1, 0);

        m_text->rerender_texture();
    }
}


bool gui::BasicTextEntry::check_clicked(int mx, int my)
{
    return common::within_rect(m_rect, mx, my);
}