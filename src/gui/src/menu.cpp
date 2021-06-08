#include "menu.h"


gui::Menu::Menu(SDL_Point pos, int menu_width, const std::vector<std::string>& options, common::Font& font, SDL_Color bg_color, SDL_Renderer* rend)
    : m_bg_color(bg_color), m_font(font)
{
    m_rect = { pos.x, pos.y, menu_width };

    for (int i = 0; i < options.size(); ++i)
    {
        m_options.emplace_back(new Text(rend, font, { pos.x, pos.y + font.char_dim().y * i }, options[i], { 255, 255, 255 }));
    }

    m_rect.h = options.size() * font.char_dim().y;
}


void gui::Menu::render(SDL_Renderer* rend, int mx, int my)
{
    SDL_SetRenderDrawColor(rend, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    for (auto& option : m_options)
    {
        option->render();
    }

    if (common::within_rect(m_rect, mx, my))
    {
        int index = (int)((my - m_rect.y) / m_font.char_dim().y);
        int rounded = m_rect.y + index * m_font.char_dim().y;

        SDL_Rect rect = { m_rect.x, rounded, m_rect.w, m_font.char_dim().y };

        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 50);
        SDL_RenderFillRect(rend, &rect);
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
    }
}


void gui::Menu::highlight_at_mouse(int mx, int my)
{
    if (!common::within_rect(m_rect, mx, my))
        return;

    int relative_my = my - m_rect.y;
    int rounded = (int)(relative_my / m_font.char_dim().y) * m_font.char_dim().y;
}