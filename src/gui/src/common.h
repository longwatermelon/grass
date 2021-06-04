#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include <string>

namespace gui::common
{
    struct TextureDeleter
    {
        void operator()(SDL_Texture* ptr)
        {
            SDL_DestroyTexture(ptr);
        }
    };

    // just to make sure i dont leak memory
    [[nodiscard]] SDL_Texture* render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color = { 255, 255, 255 });
    void center_rendered_text(SDL_Renderer* rend, SDL_Texture* tex, const char* text, SDL_Rect enclosing_rect, SDL_Point char_dim, SDL_Color color);

    bool within_rect(SDL_Rect rect, int x, int y);
}