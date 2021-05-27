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
        void render_centered(SDL_Renderer* rend, SDL_Rect rect);

        void insert(int x, int y, char c);

        /// <summary>
        /// Erases a character at m_contents[y][x].
        /// </summary>
        /// <param name="x">Char index</param>
        /// <param name="y">String index</param>
        /// <param name="erase_nl">If true it will erase new lines like regular characters, otherwise it wont do anything. Useful for when user uses the delete key.</param>
        void erase(int x, int y, bool erase_nl = true);

        void move_to(int x, int y);

        std::string get_longest_string();
        std::string& get_last_string();
        std::string get_line(int i);
        std::string& get_line_ref(int i);

        std::string str();

        std::vector<std::string> contents() const { return m_contents; }
        SDL_Point char_dim() const { return m_char_dim; }

        void set_contents(std::vector<std::string>& contents) { m_contents = contents; }
        void set_line(int i, const std::string& text) { m_contents[i] = text; }
        void insert_line(int i) { m_contents.insert(m_contents.begin() + i, ""); }

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