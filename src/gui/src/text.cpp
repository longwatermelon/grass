#include "text.h"


gui::Text::Text(SDL_Renderer* rend, common::Font& font, SDL_Point pos, const std::string& text, SDL_Color color)
    : m_rend(rend), m_font(font), m_rect{ pos.x, pos.y }, m_text(text), m_color(color)
{
    TTF_SizeText(m_font.font(), m_text.c_str(), &m_rect.w, &m_rect.h);
    m_tex = common::render_text(m_rend, m_font.font(), m_text.c_str(), m_color);
}


gui::Text::~Text()
{
    if (m_tex)
        SDL_DestroyTexture(m_tex);

    m_tex = nullptr;
}


void gui::Text::render()
{
    SDL_RenderCopy(m_rend, m_tex, 0, &m_rect);
}


void gui::Text::set_text(const std::string& text)
{
    if (m_tex)
        SDL_DestroyTexture(m_tex);

    m_text = text;
    m_tex = common::render_text(m_rend, m_font.font(), text.c_str(), m_color);

    TTF_SizeText(m_font.font(), text.c_str(), &m_rect.w, &m_rect.h);
}