#pragma once
#include <SDL.h>
#include <SDL_ttf.h>


namespace gui::common
{
    void draw_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color = { 255, 255, 255 });
    void draw_centered_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Point pos, SDL_Rect enclosing_rect, SDL_Point char_dim, SDL_Color col = { 255, 255, 255 });

    bool within_rect(SDL_Rect rect, int x, int y);
}