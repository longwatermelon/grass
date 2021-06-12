#pragma once
#include "common.h"
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>


namespace gui
{
    class String
    {
    public:
        String() = default;
        String(common::Font& font, SDL_Point pos, const std::string& contents, SDL_Color col);

        /* Inserts a character at m_contents[y][x]. */
        void insert(int x, int y, char c);

        /* Erases a character at m_contents[y][x].
        * Erases a new line like a normal character by default, but if erase_nl is set to false it will not erase new lines.
        */
        void erase(int x, int y, bool erase_nl = true);

        void remove_line(int i) { m_contents.erase(m_contents.begin() + i); }


        // getters and setters

        // Get m_contents in string form.
        std::string str();

        std::string get_line(int i) const { return i >= m_contents.size() ? "" : m_contents[i]; }
        std::string& get_line_ref(int i) { return m_contents[i]; }

        std::vector<std::string> contents() const { return m_contents; }
        void set_contents(const std::vector<std::string>& contents) { m_contents = contents; }

        void set_line(int i, const std::string& text) { m_contents[i] = text; }
        void insert_line(int i) { m_contents.insert(m_contents.begin() + i, ""); }

        SDL_Point char_dim() const { return m_font->char_dim(); }
        SDL_Point& char_dim_ref() { return m_font->char_dim_ref(); }

        TTF_Font* font() { return m_font->font(); }
        SDL_Color color() const { return m_color; }

        common::Font& font_ref() { return *m_font; }

    private:
        SDL_Rect m_rect{ 0, 0, 0, 0 };

        std::vector<std::string> m_contents = std::vector<std::string>(0);
        SDL_Color m_color{ 0, 0, 0 };

        common::Font* m_font;
    };
}
