#include "text_entry.h"
#include <functional>
#include <iostream>


gui::TextEntry::TextEntry(SDL_Rect rect, SDL_Color bg_color, const Cursor& cursor, const String& text)
    : m_rect(rect), m_cursor(cursor), m_bg_color(bg_color), m_text(text)
{
    m_highlight_start = Cursor({ -1, -1 }, { 255, 255, 255 }, m_text.char_dim_ref());

    m_min_bounds = { 0, 0 };
    m_max_bounds = { m_rect.w / m_text.char_dim().x, m_rect.h / m_text.char_dim().y };

    m_text.set_contents({ "" });
    m_cached_textures.emplace_back(std::vector<LineSection>(0));
    m_ln_textures.emplace_back(nullptr);
}


void gui::TextEntry::render(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    if (m_show_cursor && m_cursor.display_pos(m_min_bounds).y >= m_rect.y)
        m_cursor.render(rend, m_min_bounds);

    if (m_cursor.display_char_pos(m_rect, m_min_bounds).y >= 0)
        placeholder_at_cache(m_cursor.display_char_pos(m_rect, m_min_bounds).y);
    
    for (int i = 0; i < m_cached_textures.size(); ++i)
    {
        std::string line = m_text.get_line(i + m_min_bounds.y);

        if (m_min_bounds.x > (int)line.size()) // line is too far left to be seen
            continue;

        // take the section of the string from the min bounds to either the end of the line if its visible, otherwise the max bound x
        std::string visible = line.substr(m_min_bounds.x, std::min((int)line.size(), m_max_bounds.x + m_move_bounds_by) - m_min_bounds.x);

        if (visible.empty())
            continue;
        
        if (m_cached_textures[i].empty())
        {
            render_unrendered_text(visible, m_min_bounds.y + i);
        } 
        
        for (auto& section : m_cached_textures[i])
        {
            if (section.start >= visible.size())
                continue;

            if (!section.tex.get())
            {
                section.tex = std::unique_ptr<SDL_Texture, common::TextureDeleter>(common::render_text(rend, m_text.font(), visible.substr(section.start, std::min(section.count, (int)visible.size() - section.start)).c_str(), section.color));
            }

            SDL_Rect rect = {
                m_rect.x + section.start * m_text.char_dim().x,
                m_rect.y + m_text.char_dim().y * i
            };

            if (section.tex.get())
            {
                SDL_QueryTexture(section.tex.get(), nullptr, nullptr, &rect.w, &rect.h);
                SDL_RenderCopy(rend, section.tex.get(), nullptr, &rect);
            }
        }
    }

    render_line_numbers(rend);

    if (m_mode == EntryMode::HIGHLIGHT)
        draw_highlighted_areas(rend);
}


void gui::TextEntry::render_line_numbers(SDL_Renderer* rend)
{
    // only used once but if calculated m_cached_textures.size() times each frame it can eat up a lot of ram
    int limit = m_text.contents().size();

    SDL_SetRenderDrawColor(rend, BG_COLOR, 255);

    SDL_Rect rect = {
        m_rect.x - m_text.char_dim().x - (int)std::to_string(limit).size() * m_text.char_dim().x,
        m_rect.y,
        m_rect.x - rect.x,
        m_rect.h
    };

    SDL_RenderFillRect(rend, &rect);

    for (int i = 0; i < m_cached_textures.size(); ++i)
    {
        if (i + m_min_bounds.y >= limit)
            break;

        if (i >= m_ln_textures.size())
            m_ln_textures.emplace_back(nullptr);

        if (!m_ln_textures[i].get())
        {
            m_ln_textures[i] = std::unique_ptr<SDL_Texture, common::TextureDeleter>(common::render_text(rend, m_text.font(), std::to_string(i + m_min_bounds.y + 1).c_str(), { 200, 255, 240 }));
        }

        rect = {
            m_rect.x - (int)std::to_string(i + m_min_bounds.y + 1).size() * m_text.char_dim().x - m_text.char_dim().x,
            m_rect.y + m_text.char_dim().y * i
        };

        if (m_ln_textures[i].get())
        {
            SDL_QueryTexture(m_ln_textures[i].get(), 0, 0, &rect.w, &rect.h);
            SDL_RenderCopy(rend, m_ln_textures[i].get(), 0, &rect);
        }
    }
}


bool gui::TextEntry::check_clicked(int mx, int my)
{
    return common::within_rect(m_rect, mx, my) && !m_hidden;
}


void gui::TextEntry::insert_char(char c)
{
    SDL_Point cursor_coords = m_cursor.char_pos(m_rect);
    m_text.insert(cursor_coords.x, cursor_coords.y, c);

    if (c == '\n')
    {
        std::string line = m_text.get_line(cursor_coords.y);
        std::string copied = line.substr(cursor_coords.x, line.size());

        m_text.get_line_ref(cursor_coords.y).erase(cursor_coords.x, line.size());

        reset_bounds_x();

        // move the cursor to the beginning of the text box and down by 1 character
        m_cursor.move_characters(-m_cursor.char_pos(m_rect).x, 1);

        if (out_of_bounds())
        {
            move_bounds_characters(0, m_move_bounds_by);
        }

        m_text.set_line(cursor_coords.y + 1, copied);
        m_cached_textures.emplace_back(std::vector<LineSection>(0));

        clear_cache();
    }
    else
    {
        m_cursor.move_characters(1, 0);
        SDL_Point cursor_pos = m_cursor.char_pos(m_rect);

        //if (out_of_bounds_y())
        //{
        //    move_bounds_characters(0, cursor_pos.y - m_min_bounds.y);
        //    /*move_bounds_characters(m_move_bounds_by, 0);
        //    clear_cache();*/
        //}

        if (cursor_pos.y < m_min_bounds.y)
            move_bounds_characters(0, cursor_pos.y - m_min_bounds.y);
        if (cursor_pos.y >= m_max_bounds.y)
            move_bounds_characters(0, cursor_pos.y - m_max_bounds.y + 1);

        if (cursor_pos.x < m_min_bounds.x)
            move_bounds_characters(cursor_pos.x - m_min_bounds.x, 0);
        if (cursor_pos.x > m_max_bounds.x)
            move_bounds_characters(cursor_pos.x - m_max_bounds.x + m_move_bounds_by, 0);
    }
}


void gui::TextEntry::remove_char()
{
    if (m_mode == EntryMode::NORMAL)
    {
        SDL_Point cursor_coords = m_cursor.char_pos(m_rect);
        std::string line = m_text.get_line(cursor_coords.y);

        if (cursor_coords.x == 0) // backspace onto previous line
        {
            if (cursor_coords.y != 0) // dont move up if cursor is at top of the text box
            {
                if (out_of_bounds_y())
                {
                    move_bounds_characters(0, std::min(cursor_coords.y - m_min_bounds.y - 1, cursor_coords.y - m_max_bounds.y + 1));
                }

                m_text.remove_line(cursor_coords.y);

                m_cached_textures.erase(m_cached_textures.begin() + m_cursor.display_char_pos(m_rect, m_min_bounds).y);
                m_cached_textures.emplace_back(std::vector<LineSection>(0));
                placeholder_at_cache(std::max(cursor_coords.y - 1 - m_min_bounds.y, 0));

                int diff = m_text.get_line(cursor_coords.y - 1).size();
                move_cursor_characters(diff, -1);

                m_text.get_line_ref(cursor_coords.y - 1).append(line);

                if (out_of_bounds_x())
                {
                    move_bounds_characters(diff - m_move_bounds_by, 0);
                }
            }
        }
        else // normal backspace
        {
            move_cursor_characters(-1, 0);
            cursor_coords = m_cursor.char_pos(m_rect);

            m_text.erase(cursor_coords.x, cursor_coords.y);

            if (out_of_bounds_x())
            {
                move_bounds_characters(-m_move_bounds_by, 0);
            }

            if (out_of_bounds_y())
            {
                move_bounds_characters(0, cursor_coords.y - m_min_bounds.y - 1);
            }
        }
    }
    else
    {
        erase_highlighted_section();
    }

    if (m_min_bounds.y > m_text.contents().size())
        move_bounds_characters(0, m_text.contents().size() - m_min_bounds.y);
}


void gui::TextEntry::move_cursor_characters(int x, int y)
{
    SDL_Point cursor_coords = m_cursor.char_pos(m_rect);

    if (cursor_coords.x + x >= 0 && cursor_coords.y + y >= 0 && cursor_coords.y + y < m_text.contents().size())
        m_cursor.move_characters(x, y);
}


bool gui::TextEntry::jump_to_eol()
{
    SDL_Point cursor_pos = m_cursor.char_pos(m_rect);
    int eol = (int)m_text.get_line(cursor_pos.y).size();

    if (cursor_pos.x != eol)
    {
        m_cursor.move_characters(eol - cursor_pos.x, 0);

        if (out_of_bounds())
            return true;
    }

    return false;
}


bool gui::TextEntry::conditional_jump_to_eol()
{
    SDL_Point cursor_pos = m_cursor.char_pos(m_rect);
    std::string line = m_text.get_line(cursor_pos.y);

    if (cursor_pos.x > (int)line.size())
    {
        if (jump_to_eol())
        {
            move_bounds_characters(((int)line.size() - m_min_bounds.x) - 3, 0);
            clear_cache();
        }
    }

    return false;
}


void gui::TextEntry::conditional_move_bounds_characters(int x, int y)
{
    if (out_of_bounds())
    {
        move_bounds_characters(x, y);
    }
}


void gui::TextEntry::move_bounds_characters(int x, int y)
{
    m_min_bounds.x += x;
    m_max_bounds.x += x;

    bool shift = true;

    if (m_min_bounds.y + y >= 0)
    {
        if (m_min_bounds.y + y < m_text.contents().size())
        {
            m_min_bounds.y += y;
            m_max_bounds.y += y;
        }
        else
        {
            shift_cache(m_text.contents().size() - m_min_bounds.y - 1);
            shift = false;

            m_min_bounds.y += m_text.contents().size() - m_min_bounds.y - 1;
            m_max_bounds.y += m_text.contents().size() - m_min_bounds.y - 1;
        }
    }
    else
    {
        shift_cache(-m_min_bounds.y);
        shift = false;

        m_max_bounds.y -= m_min_bounds.y;
        m_min_bounds.y = 0;
    }

    m_min_bounds.x = std::max(0, m_min_bounds.x);
    m_max_bounds.x = std::max(m_rect.w / m_text.char_dim().x, m_max_bounds.x);

    if (shift)
        shift_cache(y);

    if (x != 0)
        // no way to shift the cache horizontally, that would cause text to leak out of the box
        clear_cache();
}


void gui::TextEntry::reset_bounds_x()
{
    m_min_bounds.x = 0;
    m_max_bounds.x = m_rect.w / m_text.char_dim().x;
    clear_cache();
}


void gui::TextEntry::reset_bounds_y()
{
    m_min_bounds.y = 0;
    m_max_bounds.y = m_rect.h / m_text.char_dim().y;
    clear_cache();
}


bool gui::TextEntry::out_of_bounds()
{
    return out_of_bounds_x() || out_of_bounds_y();
}


bool gui::TextEntry::out_of_bounds_x()
{
    SDL_Point display_pos_pixels = m_cursor.display_pos(m_min_bounds);

    int max_x = m_rect.x + (int)(m_rect.w / m_text.char_dim().x) * m_text.char_dim().x;
    if (display_pos_pixels.x < m_rect.x || display_pos_pixels.x > max_x)
        return true;

    return false;
}


bool gui::TextEntry::out_of_bounds_y()
{
    SDL_Point display_pos_pixels = m_cursor.display_pos(m_min_bounds);

    int max_y = m_rect.y + (int)(m_rect.h / m_text.char_dim().y) * m_text.char_dim().y - m_text.char_dim().y;
    if (display_pos_pixels.y < m_rect.y || display_pos_pixels.y > max_y)
        return true;

    return false;
}


void gui::TextEntry::clear_cache()
{
    for (auto& line : m_cached_textures)
    {
        line.clear(); 
    }
}


void gui::TextEntry::placeholder_at_cache(int i)
{
    if (i >= m_cached_textures.size())
        // just in case something goes wrong
        m_cached_textures = std::vector<std::vector<LineSection>>(i + 1);

    m_cached_textures[i] = std::vector<LineSection>(0);
}


void gui::TextEntry::update_cache()
{
    m_cached_textures.clear();
    m_cached_textures = std::vector<std::vector<LineSection>>(std::max(std::min(m_max_bounds.y, (int)m_text.contents().size()) - m_min_bounds.y + 1, 1));
    m_ln_textures = std::vector<std::unique_ptr<SDL_Texture, common::TextureDeleter>>(std::max(std::min(m_max_bounds.y, (int)m_text.contents().size()) - m_min_bounds.y + 1, 1));
}


void gui::TextEntry::shift_cache(int y)
{
    if (y > 0) // shift downwards
    {
        for (int i = 0; i < y; ++i)
        {
            m_cached_textures.erase(m_cached_textures.begin());
            m_cached_textures.emplace_back(std::vector<LineSection>(0));

            m_ln_textures.erase(m_ln_textures.begin());
            m_ln_textures.emplace_back(nullptr);
        }
    }
    else // shift upwards
    {
        for (int i = y; i < 0; ++i)
        {
            m_cached_textures.pop_back();
            m_cached_textures.insert(m_cached_textures.begin(), std::vector<LineSection>(0));

            m_ln_textures.pop_back();
            m_ln_textures.insert(m_ln_textures.begin(), nullptr);
        }
    }
}


void gui::TextEntry::mouse_down(int mx, int my)
{
    move_cursor_to_click(mx, my);

    SDL_Point coords = m_cursor.char_pos(m_rect);
    std::string line = m_text.get_line(coords.y);

    if (coords.x > line.size())
    {
        if (jump_to_eol())
        {
            move_bounds_characters(line.size() - coords.x - m_move_bounds_by, 0);
        }
    }

    start_highlight();
}


void gui::TextEntry::mouse_up()
{
    conditional_stop_highlight();
}


void gui::TextEntry::move_cursor_to_click(int mx, int my)
{
    SDL_Point coords = {
        m_min_bounds.x + (int)((mx - m_rect.x) / m_text.char_dim().x),
        m_min_bounds.y + (int)((my - m_rect.y) / m_text.char_dim().y)
    };

    // int cast to prevent coords.y from wrapping around since size() returns size_t
    if (coords.y >= (int)m_text.contents().size())
        coords.y = m_text.contents().size() - 1;

    if (coords.y < 0)
        coords.y = 0;

    int y_diff = coords.y - m_cursor.char_pos(m_rect).y;

    m_cursor.move_characters(coords.x - m_cursor.char_pos(m_rect).x, coords.y - m_cursor.char_pos(m_rect).y);

    SDL_Point cursor_pos = m_cursor.char_pos(m_rect);
    std::string line = m_text.get_line(cursor_pos.y);

    if (cursor_pos.x > (int)line.size())
    {
        jump_to_eol();
    }

    SDL_Point cursor_coords = m_cursor.display_pos(m_min_bounds);

    if (cursor_coords.x < m_rect.x)
    {
        move_bounds_characters(-m_move_bounds_by, 0);
        move_cursor_characters(m_min_bounds.x - m_cursor.char_pos(m_rect).x, 0);
    }
    else if (cursor_coords.x > (m_rect.x + m_rect.w))
    {
        move_bounds_characters(m_move_bounds_by, 0);
    }
    
    conditional_move_bounds_characters(0, y_diff);
}


void gui::TextEntry::set_cursor_pos_characters(int x, int y)
{
    SDL_Point char_pos = m_cursor.char_pos(m_rect);
    m_cursor.move_characters(x - char_pos.x, y - char_pos.y);
}


void gui::TextEntry::start_highlight()
{
    m_highlight_start = m_cursor;
    m_mode = EntryMode::HIGHLIGHT;
}


void gui::TextEntry::stop_highlight()
{
    m_mode = EntryMode::NORMAL;
    m_highlight_start.move_characters(-1 - m_highlight_start.char_pos(m_rect).x, -1 - m_highlight_start.char_pos(m_rect).y);
}


void gui::TextEntry::conditional_stop_highlight()
{
    SDL_Point cursor_pos = m_cursor.pos();
    SDL_Point start_pos = m_highlight_start.pos();

    if (cursor_pos.x == start_pos.x && cursor_pos.y == start_pos.y)
        stop_highlight();
}


void gui::TextEntry::draw_highlighted_areas(SDL_Renderer* rend)
{
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rend, 180, 180, 255, 90);

    SDL_Point cursor_char_coords = m_cursor.char_pos(m_rect);
    SDL_Point highlight_char_coords = m_highlight_start.char_pos(m_rect);

    // single line highlight
    if (cursor_char_coords.y == highlight_char_coords.y)
    {
        SDL_Point highlight_coords = m_highlight_start.pos();
        SDL_Point cursor_coords = m_cursor.pos();

        highlight_section(rend, cursor_char_coords.y, cursor_coords.x, highlight_coords.x);
    }
    else if (cursor_char_coords.y < highlight_char_coords.y) // cursor is higher than origin
    {
        // highlight everything between cursor and origin
        for (int i = cursor_char_coords.y + 1; i < highlight_char_coords.y; ++i)
        { highlight_line(rend, i);
        }

        SDL_Point cursor_coords = m_cursor.pos();
        SDL_Point highlight_coords = m_highlight_start.pos();

        highlight_section(rend, cursor_char_coords.y, cursor_coords.x, m_rect.x + (int)m_text.get_line(cursor_char_coords.y).size() * m_text.char_dim().x);
        highlight_section(rend, highlight_char_coords.y, highlight_coords.x, m_rect.x);
    }
    else // cursor is lower than origin
    {
        // highlight everything between origin and cursor
        for (int i = highlight_char_coords.y + 1; i < cursor_char_coords.y; ++i)
        {
            highlight_line(rend, i);
        }

        SDL_Point cursor_coords = m_cursor.pos();
        SDL_Point highlight_coords = m_highlight_start.pos();

        highlight_section(rend, highlight_char_coords.y, highlight_coords.x, m_rect.x + (int)m_text.get_line(highlight_char_coords.y).size() * m_text.char_dim().x);
        highlight_section(rend, cursor_char_coords.y, cursor_coords.x, m_rect.x);
    }

    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
}


void gui::TextEntry::highlight_line(SDL_Renderer* rend, int y_index)
{
    if ((y_index - m_min_bounds.y) * m_text.char_dim().y + m_rect.y < m_rect.y)
        return;

    std::string line = m_text.get_line(y_index);

    // line is not visible
    if (m_min_bounds.x > (int)line.size())
        return;

    SDL_Rect rect = {
        m_rect.x,
        (y_index - m_min_bounds.y) * m_text.char_dim().y + m_rect.y,
        std::min((int)line.size() * m_text.char_dim().x - m_min_bounds.x * m_text.char_dim().x, m_rect.w),
        m_text.char_dim().y
    };

    SDL_RenderFillRect(rend, &rect);
}


void gui::TextEntry::highlight_section(SDL_Renderer* rend, int y_index, int x1, int x2)
{
    if ((y_index - m_min_bounds.y) * m_text.char_dim().y + m_rect.y < m_rect.y)
        return;

    x1 -= m_min_bounds.x * m_text.char_dim().x;
    x2 -= m_min_bounds.x * m_text.char_dim().x;

    std::string line = m_text.get_line(y_index);

    if (m_min_bounds.x > (int)line.size())
        return;

    if (x2 < x1)
        std::swap(x1, x2);

    if (x1 == 0)
        return;

    SDL_Rect rect = {
        std::max(x1, m_rect.x),
        (y_index - m_min_bounds.y) * m_text.char_dim().y + m_rect.y,
        std::max(x2 - std::max(x1, m_rect.x), -rect.x),
        m_text.char_dim().y
    };

    SDL_RenderFillRect(rend, &rect);
}


void gui::TextEntry::erase_highlighted_section()
{
    SDL_Point cursor_char_coords = m_cursor.char_pos(m_rect);
    SDL_Point highlight_char_coords = m_highlight_start.char_pos(m_rect);

    if (cursor_char_coords.y == highlight_char_coords.y) // single line highlight
    {
        int min = std::min(cursor_char_coords.x, highlight_char_coords.x);
        int max = std::max(cursor_char_coords.x, highlight_char_coords.x);

        m_text.get_line_ref(cursor_char_coords.y).erase(min, max - min);

        if (cursor_char_coords.x > highlight_char_coords.x)
        {
            m_cursor.move_characters(highlight_char_coords.x - cursor_char_coords.x, 0);
        }

        cursor_char_coords = m_cursor.char_pos(m_rect);

        if ((int)m_text.get_line_ref(cursor_char_coords.y).size() <= cursor_char_coords.x)
            jump_to_eol();

        stop_highlight();

        if (out_of_bounds())
            move_bounds_characters(cursor_char_coords.x - m_min_bounds.x, cursor_char_coords.y - m_min_bounds.y);
    }
    else if (cursor_char_coords.y < highlight_char_coords.y) // cursor above origin
    {
        for (int i = cursor_char_coords.y + 1; i < highlight_char_coords.y;)
        {
            m_text.remove_line(i);
            --highlight_char_coords.y;
        }

        std::string& cursor_string = m_text.get_line_ref(cursor_char_coords.y);
        cursor_string.erase(cursor_char_coords.x, (int)cursor_string.size() - cursor_char_coords.x);
        
        std::string& orig_string = m_text.get_line_ref(highlight_char_coords.y);
        if (highlight_char_coords.x == (int)orig_string.size())
            m_text.remove_line(highlight_char_coords.y);
        else
            orig_string.erase(0, highlight_char_coords.x);

        if (out_of_bounds())
            move_bounds_characters(cursor_char_coords.x - m_min_bounds.x, cursor_char_coords.y - m_min_bounds.y);

        stop_highlight();
        clear_cache();
    }
    else // cursor below origin
    {
        SDL_Point original_cursor_coords = m_cursor.char_pos(m_rect);

        for (int i = highlight_char_coords.y + 1; i < cursor_char_coords.y;)
        {
            m_text.remove_line(i);
            --cursor_char_coords.y;
        }

        std::string& orig_string = m_text.get_line_ref(highlight_char_coords.y);
        orig_string.erase(highlight_char_coords.x, (int)orig_string.size() - highlight_char_coords.x);

        std::string& cursor_string = m_text.get_line_ref(cursor_char_coords.y);
        if (cursor_char_coords.x == (int)cursor_string.size())
            m_text.remove_line(cursor_char_coords.y);
        else
            cursor_string.erase(0, cursor_char_coords.x);

        stop_highlight();
        clear_cache();

        int diff_x = highlight_char_coords.x - original_cursor_coords.x;
        int diff_y = highlight_char_coords.y - original_cursor_coords.y;

        move_cursor_characters(diff_x, diff_y);

        if (out_of_bounds_x())
        {
            move_bounds_characters(diff_x + (diff_x > 0 ? 1 : -1) * m_move_bounds_by, 0);
        }

        if (out_of_bounds_y())
        {
            move_bounds_characters(0, diff_y - m_move_bounds_by);
        }
    }
}


void gui::TextEntry::resize_to(int w, int h)
{
    stop_highlight();

    m_rect.w = w - m_rect.x;
    m_rect.h = h - m_rect.y;

    m_max_bounds = {
        m_min_bounds.x + (int)(m_rect.w / m_text.char_dim().x),
        m_min_bounds.y + (int)(m_rect.h / m_text.char_dim().y)
    };

    if (m_max_bounds.y >= m_text.contents().size())
    {
        move_bounds_characters(0, m_text.contents().size() - m_max_bounds.y);
    }

    m_cursor.move_pixels(
        std::min(m_max_bounds.x * m_text.char_dim().x + m_rect.x, m_cursor.pos().x) - m_cursor.pos().x,
        std::min(m_max_bounds.y * m_text.char_dim().y + m_rect.y, m_cursor.pos().y) - m_cursor.pos().y
    );

    conditional_jump_to_eol();
    update_cache();
}


size_t gui::TextEntry::get_tab_position()
{
    std::string line = m_text.get_line(m_cursor.char_pos(m_rect).y);

    for (int i = 0; i < line.size(); ++i)
    {
        if (line[i] != ' ')
            return i;
    }

    return line.size();
}


void gui::TextEntry::resize_text(int size)
{
    m_show_cursor = false;
    m_cursor.move_pixels(m_rect.x - m_cursor.pos().x, m_rect.y - m_cursor.pos().y);

    common::Font& f = m_text.font_ref();

    if (size > 5 && size < 30)
        f.change_pt_size(size);

    resize_to(m_rect.x + m_rect.w, m_rect.y + m_rect.h);
    update_cache();
}


void gui::TextEntry::highlight_text(int y, int start, int count, SDL_Color color)
{
    m_cached_textures[y - m_min_bounds.y].emplace_back(LineSection{ start, count, nullptr, color });
}


void gui::TextEntry::render_unrendered_text(const std::string& visible, int y)
{
    std::vector<LineSection>& sections = m_cached_textures[y - m_min_bounds.y];

    std::function<bool(int)> occupied = [&](int x) {
        for (auto& s : sections)
        {
            if (x >= s.start && x <= s.start + s.count)
            {
                return true;
            }
        }

        return false;
    };

    for (int i = 0; i < visible.size(); ++i)
    {
        if (occupied(i))
            continue;
        
        int start = i - 1;
        start = std::max(start, 0);

        while (i < visible.size() && !occupied(i))
        {
            ++i;
        }

        highlight_text(y, start, i - start, { 255, 255, 255 });
    }
}


void gui::TextEntry::highlight_all_occurrences(int y, const std::string& text, SDL_Color color)
{
    std::string line = m_text.get_line(y);
    size_t pos = line.find(text);

    while (pos != std::string::npos)
    {
        highlight_text(y, pos, 2, color);
        pos = line.find(text, pos + text.size());
    }
}
