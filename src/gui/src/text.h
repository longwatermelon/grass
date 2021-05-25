#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <string>


namespace gui
{
    class Text
    {
    public:
        Text(TTF_Font* font, SDL_Point pos, const std::string& contents, SDL_Point char_dimensions, SDL_Color col);

    public:
        void render(SDL_Renderer* rend);
        void render_centered(SDL_Renderer* rend, SDL_Rect rect);

        void move_to(int x, int y);

        std::string str() const { return m_contents; }

    private:
        SDL_Rect m_rect;

        SDL_Point m_char_dim;
        std::string m_contents;
        SDL_Color m_color;

        // non owning, dont free
        TTF_Font* m_font;
    };
}