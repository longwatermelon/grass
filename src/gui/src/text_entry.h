#pragma once
#include "text.h"
#include "cursor.h"
#include "common.h"
#include <memory>


namespace gui
{
    class TextEntry
    {
    public:
        TextEntry(SDL_Rect rect, SDL_Color bg_color, const Cursor& cursor, const Text& text);

        void render(SDL_Renderer* rend);

        bool check_clicked(int mx, int my);

        // adds a character to where the cursor currently is
        void insert_char(char c);
        // removes a character where the cursor currently is
        void remove_char();

        void move_cursor_characters(int x, int y);
        
        // moves bounds by (x, y) characters
        void move_bounds_characters(int x, int y);

        void reset_bounds_x();
        void reset_bounds_y();

        void clear_cache();
        // marks m_cached_textures[i] to be re rendered
        void placeholder_at_cache(int i);
        // resize and clear the cache to fit current text size
        void update_cache();
        // move the cache vertically by y characters
        void shift_cache(int y);


        Text* text() { return &m_text; }

    private:
        SDL_Rect m_rect;
        Cursor m_cursor;
        SDL_Color m_bg_color;

        // measured in characters
        SDL_Point m_min_bounds, m_max_bounds;

        Text m_text;

        std::vector<std::unique_ptr<SDL_Texture, common::TextureDeleter>> m_cached_textures;
    };
}
