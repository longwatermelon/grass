#include "tab.h"


gui::Tab::Tab(std::unique_ptr<Text> text, SDL_Color color, const std::string& full_path, int rect_h)
    : m_text(std::move(text)), m_full_path(full_path), m_rect(text->rect())
{
    m_rect.h = rect_h;
}


void gui::Tab::render(SDL_Renderer* rend)
{
    SDL_Color bg_color = { 0, 0, 0 };

    if (m_hover)
    {
        bg_color.r += 60; 
        bg_color.g += 60;
        bg_color.b += 60;
    }

    if (m_clicked)
    {
        bg_color.r += 100;
        bg_color.g += 100;
        bg_color.b += 100;
    }

    SDL_SetRenderDrawColor(rend, bg_color.r, bg_color.g, bg_color.b, 255);
    // SDL_Rect rect = m_text->rect();
    SDL_RenderFillRect(rend, &m_rect);

    m_text->render(); 
}


int gui::Tab::text_pixel_length()
{
    return m_text->text().size() * m_text->font_ref().char_dim().x;
}


bool gui::Tab::check_clicked(int mx, int my)
{
    return common::within_rect(m_rect, mx, my);
}


void gui::Tab::hover_highlight(int mx, int my)
{
    m_hover = common::within_rect(m_rect, mx, my);
}


void gui::Tab::move(int x)
{
    m_text->move(x, 0);
    m_rect.x += x;
}
