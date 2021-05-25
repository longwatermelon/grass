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

        void append(char c);
        void pop_back();

        void move_to(int x, int y);

        std::string get_longest_string();
        std::string& get_last_string();

        std::string str();

        std::vector<std::string> contents() const { return m_contents; }
        SDL_Point char_dim() const { return m_char_dim; }

    private:
        SDL_Rect m_rect;

        SDL_Point m_char_dim;
        std::vector<std::string> m_contents;
        SDL_Color m_color;

        // non owning, dont free
        TTF_Font* m_font;
    };
}