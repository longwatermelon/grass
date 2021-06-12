#include "gui_string.h"
#include "common.h"
#include <sstream>


gui::String::String(common::Font& font, SDL_Point pos, const std::string& contents, SDL_Color col)
    : m_font(&font), m_color(col)
{
    m_rect = { pos.x, pos.y, m_font->char_dim().x * (int)contents.size(), m_font->char_dim().y };

    std::string line;
    std::stringstream ss(contents);
    while (std::getline(ss, line)) m_contents.emplace_back(line);

    if (m_contents.empty())
        m_contents.emplace_back("");
}


void gui::String::insert(int x, int y, char c)
{
    if (m_contents.size() == 0)
        m_contents.emplace_back("");

    if (c == '\n')
        m_contents.insert(m_contents.begin() + y + 1, "");
    else
        m_contents[y].insert(m_contents[y].begin() + x, c);
}


void gui::String::erase(int x, int y, bool erase_nl)
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


std::string gui::String::str()
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