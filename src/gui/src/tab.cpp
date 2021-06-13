#include "tab.h"


gui::Tab::Tab(std::unique_ptr<Text> text, SDL_Color color, const std::string& full_path)
    : m_text(std::move(text)), m_full_path(full_path) {}


void gui::Tab::render(SDL_Renderer* rend)
{
    m_text->render(); 
}


int gui::Tab::text_pixel_length()
{
    return m_text->text().size() * m_text->font_ref().char_dim().x;
}


bool gui::Tab::check_clicked(int mx, int my)
{
    return common::within_rect(m_text->rect(), mx, my);
}
