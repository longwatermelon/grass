#pragma once
#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>


namespace gui
{
    class Text
    {
    public:
        Text(TTF_Font* font, SDL_Point pos, const std::string& contents, SDL_Point char_dimensions, SDL_Color col);

    public:
        void render(SDL_Renderer* rend);

        /* Render text in the center of a rectangle. */
        void render_centered(SDL_Renderer* rend, SDL_Rect rect);

        /* Inserts a character at m_contents[y][x]. */
        void insert(int x, int y, char c);

        /* Erases a character at m_contents[y][x].
        * Erases a new line like a normal character by default, but if erase_nl is set to false it will not erase new lines.
        */
        void erase(int x, int y, bool erase_nl = true);


        // getters and setters

        // Get m_contents in string form.
        std::string str();

        std::string get_line(int i) const { return m_contents[i]; }
        std::string& get_line_ref(int i) { return m_contents[i]; }

        std::vector<std::string> contents() const { return m_contents; }
        void set_contents(std::vector<std::string>& contents) { m_contents = contents; }

        void set_line(int i, const std::string& text) { m_contents[i] = text; }
        void insert_line(int i) { m_contents.insert(m_contents.begin() + i, ""); }

        SDL_Point char_dim() const { return m_char_dim; }

    private:
        SDL_Rect m_rect;

        // character dimensions
        SDL_Point m_char_dim;
        std::vector<std::string> m_contents;
        SDL_Color m_color;

        // non owning, dont free
        TTF_Font* m_font;
    };
}