#include "text.h"
#include <memory>


gui::Text::Text(SDL_Renderer* rend, common::Font& font, SDL_Point pos, const std::string& text, SDL_Color color)
    : m_rend(rend), m_font(font), m_rect{ pos.x, pos.y }, m_text(text), m_color(color)
{
    TTF_SizeText(m_font.font(), m_text.c_str(), &m_rect.w, &m_rect.h);
    m_tex = std::unique_ptr<SDL_Texture, common::TextureDeleter>(common::render_text(m_rend, m_font.font(), m_text.c_str(), m_color));
}


void gui::Text::render()
{
    if (m_tex)
        SDL_RenderCopy(m_rend, m_tex.get(), 0, &m_rect);
}


void gui::Text::rerender_texture()
{
    set_text(m_text);
}


void gui::Text::set_text(const std::string& text)
{
    m_text = text;
    m_tex = std::unique_ptr<SDL_Texture, common::TextureDeleter>(common::render_text(m_rend, m_font.font(), text.c_str(), m_color));

    TTF_SizeText(m_font.font(), text.c_str(), &m_rect.w, &m_rect.h);
}


void gui::Text::move(int x, int y)
{
    m_rect.x += x;
    m_rect.y += y;
}
