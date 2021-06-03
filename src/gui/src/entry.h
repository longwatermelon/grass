#pragma once
#include "text.h"
#include "common.h"
#include <memory>


namespace gui
{
    class TextEntry
    {
    public:
        TextEntry(SDL_Rect rect, const Text& text, SDL_Color bg_col = { 255, 255, 255 }, SDL_Color cursor_col = { 0, 0, 0 });

    public:
        void render(SDL_Renderer* rend);

        /* Add a character where the cursor currently is. */
        void add_char(char c);

        /* Remove count number of characters where the cursor currently is. */
        void remove_char(int count);

        /* Checks if mx, my is inside of m_rect. */
        bool check_clicked(int mx, int my);

        /* Moves both real and display cursors. 
        * Automatically checks for bounds after moving cursors, can be turned off by setting parameter check to false.
        */
        void move_cursor(int x, int y, bool check = true);

        /* Converts pixel coordinates to indexes
        * Returned y value is the index of the string in the vector
        * Returned x value is the index of the character in the string
        */
        SDL_Point real_to_char_pos(SDL_Point pos);

        /* Moves minimum and maximum visible bounds by (x, y). Arguments are measured in indexes not pixel coordinates.
        * Returns true if the bounds moved successfully, otherwise false
        */
        bool move_bounds(int x, int y);
        /* Reset horizontal visibility to default */
        void reset_bounds_x();
        /* Reset vertical visibility to default */
        void reset_bounds_y();
        /* Set both display and real cursor positions to (x, y), doesnt perform any additional checks. Parameters are measured as indexes */
        void set_cursor_pos(int x, int y);

        /* Moves the visible boundaries by (x, y) if the display cursor is outside of the text box, then moves the display cursor back inside.
        * x: x displacement if the display cursor x < m_rect.x or > m_rect.x + m_rect.w
        * y: y displacement if the display cursor y < m_rect.y or >= m_rect.y + m_rect.h
        * X and y are measured in indexes, not pixel coordinates.
        * Returns true if the bounds were moved, otherwise false
        */
        bool check_bounds(int x, int y);

        /* Moves the display cursors position. Parameters measured in characters
        */
        void move_display_cursor(int x, int y);
        /* Moves the real cursors position. Parameters measured in characters
        * Protects against moving lower than the amount of text stored.
        */
        void move_real_cursor(int x, int y);

        /* Moves real and display cursors to eol, automatically checks for bounds but can be toggled with the check parameter. */
        void jump_to_eol(bool check = true);

        void draw_cursor(SDL_Renderer* rend);


        /* Sets m_cached_texutres[index - m_cache_offset] to nullptr. Automatically appends new element if index doesnt exist */
        void placeholder_at_cache(int index);
        /* Moves the cache offset by y characters and then removes textures outside of the visible zone */
        void clear_cache();
        /* Remove texture from m_cached_textures[index]. */
        void remove_texture_from_cache(int index);
        /* Clears and re renders the entire cache using m_text.contents() */
        void update_cache();
        /* shifts cached textures vertically by y indexes */
        void shift_cache(int y);

        /* Sets window width to w and height to h */
        void resize_to(int w, int h);


        void scroll(int y);

        bool cursor_visible();

        void move_cursor_to_click(int mx, int my);


        // getter functions

        Text* text() { return &m_text; }
        SDL_Point display() { return m_display_cursor_pos; }
        SDL_Point real() { return m_real_cursor_pos; }
        std::string get_current_line() { return m_text.get_line((m_real_cursor_pos.y - m_rect.y) / m_text.char_dim().y); }
        SDL_Point get_coords() { return real_to_char_pos(m_real_cursor_pos); }

    private:
        SDL_Rect m_rect;
        Text m_text;
        SDL_Color m_background_color;

        SDL_Point m_display_cursor_pos;
        SDL_Point m_real_cursor_pos;

        SDL_Point m_min_visible_indexes, m_max_visible_indexes;
        std::vector<std::unique_ptr<SDL_Texture, common::TextureDeleter>> m_cached_textures;

        SDL_Color m_cursor_color;
    };
}