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
        std::string visible = line.substr(m_min_bounds.x, std::min((int)line.size(), m_max_bounds.x + 1) - m_min_bounds.x);

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

        // if the string was an empty string it is still possible that code execution got here but the cached texture is nullptr
        if (m_cached_textures[i].get())
        {
            SDL_RenderCopy(rend, m_cached_textures[i].get(), nullptr, &rect);
        }
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

        m_text.set_line(cursor_coords.y + 1, copied);
        m_cached_textures.emplace_back(nullptr);

        clear_cache();
    }
    else
    {
        m_cursor.move_characters(1, 0);
    }
}


void gui::TextEntry::remove_char()
{
}


void gui::TextEntry::move_cursor_characters(int x, int y)
{
    m_cursor.move_characters(x, y);
}


void gui::TextEntry::move_bounds_characters(int x, int y)
{
    m_min_bounds.x += x;
    m_max_bounds.x += x;

    m_min_bounds.y += y;
    m_max_bounds.y += y;
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