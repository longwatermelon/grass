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
    common::draw_centered_text(rend, m_font, str().c_str(), { m_rect.x, m_rect.y }, rect, m_char_dim, m_color);
}


void gui::Text::append(char c)
{
    get_last_string() += c;

    if (c == '\n')
    {
        get_last_string().pop_back();

        m_contents.emplace_back("");
        m_rect.h += m_char_dim.y;
    }
    else
    {
        if (get_longest_string() == get_last_string())
            m_rect.w += m_char_dim.x;
    }
}


void gui::Text::pop_back()
{
    std::string& last_string = get_last_string();

    if (last_string.size() > 0)
        last_string.pop_back();

    if (last_string.size() == 0)
        m_contents.pop_back();

    if (m_contents.size() == 0)
        m_contents.emplace_back("");
}


void gui::Text::move_to(int x, int y)
{
    m_rect.x = x;
    m_rect.y = y;
}


std::string gui::Text::get_longest_string()
{
    std::string largest;

    for (auto& str : m_contents)
    {
        if (str.size() > largest.size())
            largest = str;
    }

    return largest;
}


std::string& gui::Text::get_last_string()
{
    return m_contents[m_contents.size() - 1];
}


std::string gui::Text::str()
{
    std::string s;

    for (auto& line : m_contents)
    {
        s += line + '\n';
    }

    s.pop_back();
    return s;
}