#pragma once
#include "text.h"
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>


namespace gui
{
    class TextEntry
    {
    public:
        TextEntry(SDL_Rect rect, const Text& text, SDL_Color bg_col = { 255, 255, 255 });

    public:
        void render(SDL_Renderer* rend);

        /* Add a character where the cursor currently is. */
        void add_char(char c);

        /* Remove count number of characters where the cursor currently is. */
        void remove_char(int count);

        /* Checks if mx, my is inside of m_rect. */
        bool check_clicked(int mx, int my);

        /* Gets all visible text from the minimum and maximum visible boundaries. */
        std::vector<std::string> get_visible_content();

        /* Moves both real and display cursors. 
        * Automatically checks for bounds after moving cursors, can be turned off by setting parameter check to false.
        */
        void move_cursor(int x, int y, bool check = true);

        /* Converts pixel coordinates to indexes
        * Returned y value is the index of the string in the vector
        * Returned x value is the index of the character in the string
        */
        SDL_Point real_to_char_pos(SDL_Point pos);

        /* Moves minimum and maximum visible bounds by (x, y). Arguments are measured in indexes not pixel coordinates */
        void move_bounds(int x, int y);
        /* Reset horizontal visibility to default */
        void reset_bounds_x();
        /* Reset vertical visibility to default */
        void reset_bounds_y();

        /* Moves the visible boundaries by (x, y) if the display cursor is outside of the text box, then moves the display cursor back inside.
        * x: x displacement if the display cursor x < m_rect.x or > m_rect.x + m_rect.w
        * y: y displacement if the display cursor y < m_rect.y or >= m_rect.y + m_rect.h
        */
        void check_bounds(int x, int y);

        /* Moves the display cursors position.
        * Protects against moving lower than the amount of text stored.
        */
        void move_display_cursor(int x, int y);
        /* Moves the real cursors position. 
        * Protects against moving lower than the amount of text stored.
        */
        void move_real_cursor(int x, int y);

        /* Moves real and display cursors to eol, automatically checks for bounds but can be toggled with the check parameter. */
        void jump_to_eol(bool check = true);

        void draw_cursor(SDL_Renderer* rend);


        // getter functions

        Text* text() { return &m_text; }
        SDL_Point display() { return m_display_cursor_pos; }
        SDL_Point real() { return m_real_cursor_pos; }

    private:
        SDL_Rect m_rect;
        Text m_text;
        SDL_Color m_background_color;

        SDL_Point m_display_cursor_pos;
        SDL_Point m_real_cursor_pos;

        SDL_Point m_min_visible_indexes, m_max_visible_indexes;
        std::vector<std::string> m_visible_content;
    };
}