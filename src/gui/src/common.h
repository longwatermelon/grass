#pragma once
#include <SDL.h>
#include <SDL_ttf.h>


namespace gui::common
{
    void draw_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color = { 255, 255, 255 });
}