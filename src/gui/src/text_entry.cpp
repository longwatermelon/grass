#include "text_entry.h"


gui::TextEntry::TextEntry(SDL_Rect rect, SDL_Color bg_color, const Cursor& cursor, const Text& text)
    : m_rect(rect), m_cursor(cursor), m_bg_color(bg_color), m_text(text)
{
    m_min_bounds = { 0, 0 };
    m_max_bounds = { m_rect.w / m_text.char_dim().x, m_rect.h / m_text.char_dim().y };

    m_text.set_contents({ "" });
    m_cached_textures.emplace_back(nullptr);
}


void gui::TextEntry::render(SDL_Renderer* rend)
{
    SDL_SetRenderDrawColor(rend, m_bg_color.r, m_bg_color.g, m_bg_color.b, 255);
    SDL_RenderFillRect(rend, &m_rect);

    m_cursor.render(rend, m_min_bounds);

    placeholder_at_cache(m_cursor.display_char_pos(m_rect, m_min_bounds).y);

    for (int i = 0; i < m_cached_textures.size(); ++i)
    {
        std::string line = m_text.get_line(i + m_min_bounds.y);

        if (m_min_bounds.x > line.size()) // line is too far left to be seen
            continue;

        // take the section of the string from the min bounds to either the end of the line if its visible, otherwise the max bound x
        std::string visible = line.substr(m_min_bounds.x, std::min((int)line.size(), m_max_bounds.x + m_move_bounds_by) - m_min_bounds.x);

        if (visible.empty())
            continue;

        if (!m_cached_textures[i].get())
        {
            m_cached_textures[i] = std::unique_ptr<SDL_Texture, common::TextureDeleter>(common::render_text(rend, m_text.font(), visible.c_str(), m_text.color()));
        }

        SDL_Rect rect = {
            m_rect.x,
            m_rect.y + m_text.char_dim().y * i,
            m_text.char_dim().x * (int)visible.size(),
            m_text.char_dim().y
        };

        SDL_RenderCopy(rend, m_cached_textures[i].get(), nullptr, &rect);
    }
}


bool gui::TextEntry::check_clicked(int mx, int my)
{
    return common::within_rect(m_rect, mx, my);
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
        m_cached_textures.emplace_back(nullptr);

        clear_cache();
    }
    else
    {
        m_cursor.move_characters(1, 0);

        if (out_of_bounds())
        {
            move_bounds_characters(m_move_bounds_by, 0);
            clear_cache();
        }
    }
}


void gui::TextEntry::remove_char()
{
    SDL_Point cursor_coords = m_cursor.char_pos(m_rect);
    std::string line = m_text.get_line(cursor_coords.y);

    if (line.empty()) // backspace onto previous line
    {
        if (cursor_coords.y != 0) // dont move up if cursor is at top of the text box
        {
            int diff = m_text.get_line(cursor_coords.y - 1).size() - cursor_coords.x; // cursor x will probably be 0 but add it in just to be safe
            move_cursor_characters(diff, -1);
        }
    }
    else // normal backspace
    {
        move_cursor_characters(-1, 0);
        cursor_coords = m_cursor.char_pos(m_rect);

        m_text.erase(cursor_coords.x, cursor_coords.y);
    }
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
    int eol = m_text.get_line(cursor_pos.y).size();

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

    if (cursor_pos.x > line.size())
    {
        if (jump_to_eol())
        {
            move_bounds_characters((line.size() - m_min_bounds.x) - 3, 0);
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

    m_min_bounds.y += y;
    m_max_bounds.y += y;

    m_min_bounds.x = std::max(0, m_min_bounds.x);
    m_min_bounds.y = std::max(0, m_min_bounds.y);

    if (x == 0)
        shift_cache(y);
    else
        // no way to shift the cache horizontally, that would cause text to leak out of the box
        clear_cache();
}


void gui::TextEntry::reset_bounds_x()
{
    m_min_bounds.x = 0;
    m_max_bounds.x = m_rect.w / m_text.char_dim().x;
}


void gui::TextEntry::reset_bounds_y()
{
    m_min_bounds.y = 0;
    m_max_bounds.y = m_rect.h / m_text.char_dim().y;
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

    int max_y = m_rect.y + (int)(m_rect.h / m_text.char_dim().y) * m_text.char_dim().y;
    if (display_pos_pixels.y < m_rect.y || display_pos_pixels.y > max_y)
        return true;

    return false;
}


void gui::TextEntry::clear_cache()
{
    for (auto& tex : m_cached_textures)
    {
        tex = nullptr;
    }
}


void gui::TextEntry::placeholder_at_cache(int i)
{
    m_cached_textures[i] = nullptr;
}


void gui::TextEntry::update_cache()
{
    m_cached_textures.clear();
    m_cached_textures = std::vector<std::unique_ptr<SDL_Texture, common::TextureDeleter>>(std::max(std::min(m_max_bounds.y, (int)m_text.contents().size()) - m_min_bounds.y + 1, 1));
}


void gui::TextEntry::shift_cache(int y)
{
    if (y > 0) // shift downwards
    {
        for (int i = 0; i < y; ++i)
        {
            m_cached_textures.erase(m_cached_textures.begin());
            m_cached_textures.emplace_back(nullptr);
        }
    }
    else // shift upwards
    {
        for (int i = y; i < 0; ++i)
        {
            m_cached_textures.pop_back();
            m_cached_textures.insert(m_cached_textures.begin(), nullptr);
        }
    }
}