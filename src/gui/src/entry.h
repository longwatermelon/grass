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
        void add_char(char c);
        void remove_char(int count);

        bool check_clicked(int mx, int my);

        /// <summary>
        /// Gets all visible content determined by the bounds
        /// </summary>
        /// <returns>Vector of strings to be displayed in render()</returns>
        std::vector<std::string> get_visible_content();

        /// <summary>
        /// Convenient method to move both the real and displayed cursors in one method.
        /// Automatically checks for bounds after moving the cursors
        /// </summary>
        void move_cursor(int x, int y);

        /// <summary>
        /// Converts pixel coordinates to indexes.
        /// </summary>
        /// <param name="pos">The position measured in pixels</param>
        /// <returns>
        /// Y is the index of the string in the vector
        /// X is the index of the character in the string
        /// </returns>
        SDL_Point real_to_char_pos(SDL_Point pos);

        void move_bounds(int x, int y);
        void reset_bounds_x();
        void reset_bounds_y();

        /// <summary>
        /// Moves the visible boundaries by (x, y) if the display cursor is about to move outside of the text box.
        /// </summary>
        /// <param name="x">X displacement if the display cursor is moving too far horizontally</param>
        /// <param name="y">Y displacement if the display cursor is moving too far vertically</param>
        void check_bounds(int x, int y);

        /// <summary>
        /// Moves the displayed cursor's position.
        /// Attaches to end of line if the x position is longer than the current line.
        /// </summary>
        void move_display_cursor(int x, int y);

        /// <summary>
        /// Moves the real cursor's position.
        /// If real cursor coordinates go below zero they are set to 0.
        /// Attaches to end of line if the x position is longer than the current line.
        /// </summary>
        void move_real_cursor(int x, int y);

        /// <summary>
        /// Moves the real and display cursors to the end of the line, automatically checks for bounds.
        /// </summary>
        void jump_to_eol();

        void draw_cursor(SDL_Renderer* rend);

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