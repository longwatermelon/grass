#include "entry.h"
#include "common.h"
#include <iostream>


gui::TextEntry::TextEntry(SDL_Rect rect, int line_limit, const Text& text, bool show_line_numbers, SDL_Color bg_col)
    : m_rect(rect), m_line_limit(line_limit), m_text(text), m_show_line_numbers(show_line_numbers), m_background_color(bg_col)
{
    m_cursor_pos = { m_rect.x, m_rect.y };
    m_real_cursor_pos = { m_rect.x, m_rect.y };

    m_min_visible = { 0, 0 };
    m_max_visible = { m_rect.w / m_text.char_dim().x, m_rect.h / m_text.char_dim().y };
}


void gui::TextEntry::render(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, m_background_color.r, m_background_color.g, m_background_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    m_visible_content = get_visible_content();

    Text tmp = m_text;
    tmp.set_contents(m_visible_content);

    tmp.render(rend);
    std::cout << m_real_cursor_pos.x << " | " << m_real_cursor_pos.y << "\n";
}


void gui::TextEntry::add_char(char c)
{
    SDL_Point coords = real_to_char_pos(m_real_cursor_pos);
    m_text.insert(coords.x, coords.y, c);
    move_cursor(1, 0);

    if (c == '\n')
    {
        reset_bounds_x();
        move_cursor(-((m_cursor_pos.x - m_rect.x) / m_text.char_dim().x), 1);
    }

    m_visible_content = get_visible_content();
}


void gui::TextEntry::remove_char(int count)
{
    for (int i = 0; i < count; ++i)
    {
        bool new_line = false;

        if (m_text.get_line(real_to_char_pos(m_real_cursor_pos).y).size() == 0)
        {
            if (m_text.contents().size() > 1)
            {
                // move cursor up after erasing to make sure that the empty line is erased
                new_line = true;

                int diff = m_text.contents()[m_text.contents().size() - 2].size() * m_text.char_dim().x;
                m_cursor_pos.x += diff;
                m_real_cursor_pos.x += diff;

                if (diff > m_rect.w)
                {
                    int tmp = m_max_visible.x;
                    m_max_visible.x = diff / m_text.char_dim().x;
                    m_min_visible.x += m_max_visible.x - tmp;
                }
            }
        }
        else
        {
            move_cursor(-1, 0);
        }

        SDL_Point coords = real_to_char_pos(m_real_cursor_pos);
        m_text.erase(coords.x, coords.y);

        if (new_line)
            move_cursor(0, -1);
    }

    m_visible_content = get_visible_content();
}


bool gui::TextEntry::check_clicked(int mx, int my)
{
    return common::within_rect(m_rect, mx, my);
}


std::vector<std::string> gui::TextEntry::get_visible_content()
{
    std::vector<std::string> visible;

    for (int i = 0; i < m_text.contents().size(); ++i)
    {
        if (i < m_min_visible.y || i > m_max_visible.y)
            continue;

        std::string line;

        for (int j = 0; j < m_text.contents()[i].size(); ++j)
        {
            if (j >= m_min_visible.x && j <= m_max_visible.x)
            {
                line += m_text.contents()[i][j];
            }
        }

        visible.emplace_back(line);
    }

    return visible;
}


void gui::TextEntry::move_cursor(int x, int y)
{
    m_real_cursor_pos.x += x * m_text.char_dim().x;
    m_real_cursor_pos.y += y * m_text.char_dim().y;

    m_cursor_pos.x += x * m_text.char_dim().x;
    m_cursor_pos.y += y * m_text.char_dim().y;

    if (m_cursor_pos.x < m_rect.x || m_cursor_pos.x > m_rect.x + m_rect.w)
        move_bounds(x, 0);

    if (m_cursor_pos.y < m_rect.y || m_cursor_pos.y >= m_rect.y + m_rect.h)
        move_bounds(0, y);

    m_cursor_pos = {
        std::min(std::max(m_cursor_pos.x, m_rect.x), m_rect.x + m_rect.w),
        std::min(std::max(m_cursor_pos.y, m_rect.y), m_rect.y + m_rect.h - m_text.char_dim().y)
    };

    m_real_cursor_pos = {
        std::max(m_rect.x, m_real_cursor_pos.x),
        std::max(m_rect.y, m_real_cursor_pos.y)
    };
}

SDL_Point gui::TextEntry::real_to_char_pos(SDL_Point pos)
{
    return {
        (pos.x - m_rect.x) / m_text.char_dim().x,
        (pos.y - m_rect.y) / m_text.char_dim().y
    };
}


void gui::TextEntry::move_bounds(int x, int y)
{
    m_min_visible.x += x;
    m_min_visible.y += y;

    m_max_visible.x += x;
    m_max_visible.y += y;
}


void gui::TextEntry::reset_bounds_x()
{
    m_min_visible.x = 0;
    m_max_visible.x = m_rect.w / m_text.char_dim().x;
}


void gui::TextEntry::reset_bounds_y()
{
    m_min_visible.y = 0;
    m_max_visible.y = m_rect.h / m_text.char_dim().y;
}


void gui::TextEntry::draw_cursor(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
    SDL_RenderDrawLine(rend, m_cursor_pos.x, m_cursor_pos.y, m_cursor_pos.x, m_cursor_pos.y + m_text.char_dim().y);
}