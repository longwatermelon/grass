#include "text.h"
#include "common.h"
#include <sstream>


gui::Text::Text(TTF_Font* font, SDL_Point pos, const std::string& contents, SDL_Point char_dimensions, SDL_Color col)
    : m_font(font), m_char_dim(char_dimensions), m_color(col)
{
    m_rect = { pos.x, pos.y, char_dimensions.x * (int)contents.size(), char_dimensions.y };

    std::string line;
    std::stringstream ss(contents);
    while (std::getline(ss, line)) m_contents.emplace_back(line);

    if (m_contents.empty())
        m_contents.emplace_back("");
}


void gui::Text::render(SDL_Renderer* rend)
{
    for (int i = 0; i < m_contents.size(); ++i)
    {
        std::string& s = m_contents[i];

        common::draw_text(
            rend,
            m_font,
            s.c_str(), 
            { m_rect.x, m_rect.y + m_char_dim.y * i, m_char_dim.x * (int)s.size(), m_char_dim.y }, 
            m_color
        );
    }
}


void gui::Text::render_centered(SDL_Renderer* rend, SDL_Rect rect)
{
    std::string s = str();

    if (s.size() == 0)
        return;

    common::draw_centered_text(rend, m_font, s.c_str(), { m_rect.x, m_rect.y }, rect, m_char_dim, m_color);
}


void gui::Text::insert(int x, int y, char c)
{
    if (c == '\n')
        m_contents.insert(m_contents.begin() + y + 1, "");
    else
        m_contents[y].insert(m_contents[y].begin() + x, c);
}


void gui::Text::erase(int x, int y, bool erase_nl)
{
    if (m_contents[y].empty())
    {
        if (!erase_nl)
            return;

        if (m_contents.size() > 1)
            m_contents.erase(m_contents.begin() + y);
    }
    else
    {
        m_contents[y].erase(m_contents[y].begin() + x);
    }
}


std::string gui::Text::str()
{
    std::string s;

    for (auto& line : m_contents)
    {
        s += line + '\n';
    }

    if (s.size() == 0)
        return "";

    s.pop_back();
    return s;
}