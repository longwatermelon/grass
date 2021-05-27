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

        /// <summary>
        /// Add a character where the cursor currently is.
        /// </summary>
        /// <param name="c">The character to be inserted</param>
        void add_char(char c);

        /// <summary>
        /// Remove a character where the cursor currently is.
        /// </summary>
        /// <param name="count">Number of characters to be removed</param>
        void remove_char(int count);


        /// <summary>
        /// Checks if mouse is inside of m_rect.
        /// </summary>
        /// <param name="mx">Mouse x pos</param>
        /// <param name="my">Mouse y pos</param>
        /// <returns>If the mouse is inside of m_rect</returns>
        bool check_clicked(int mx, int my);

        /// <summary>
        /// Gets all visible content determined by the bounds
        /// </summary>
        /// <returns>Vector of strings to be displayed in render()</returns>
        std::vector<std::string> get_visible_content();

        /// <summary>
        /// Convenient method to move both the real and displayed cursors in one method.<para/>
        /// Automatically checks for bounds after moving the cursors.<para/>
        /// Doesnt execute if moving the cursor would go outside of the text.
        /// </summary>
        void move_cursor(int x, int y, bool check = true);

        /// <summary>
        /// Converts pixel coordinates to indexes.
        /// </summary>
        /// <param name="pos">The position measured in pixels</param>
        /// <returns>
        /// Y is the index of the string in the vector
        /// X is the index of the character in the string
        /// </returns>
        SDL_Point real_to_char_pos(SDL_Point pos);

        /// <summary>
        /// Moves visibility boundaries by (x, y)
        /// </summary>
        void move_bounds(int x, int y);

        /// <summary>
        /// Resets horizontal visibility bounds to defaults
        /// </summary>
        void reset_bounds_x();

        /// <summary>
        /// Resets vertical visibility bounds to defaults
        /// </summary>
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
        /// Moves the real cursor's position.<para/>
        /// If real cursor coordinates go below zero they are set to 0.<para/>
        /// Attaches to end of line if the x position is longer than the current line.<para/>
        /// </summary>
        void move_real_cursor(int x, int y);

        /// <summary>
        /// Moves the real and display cursors to the end of the line, automatically checks for bounds.
        /// </summary>
        /// <param name="check">Check for bounds, defaults to true</param>
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