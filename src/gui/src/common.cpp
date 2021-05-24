#include "common.h"


void gui::common::draw_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color)
{
    SDL_Surface* surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_RenderCopy(rend, tex, nullptr, &rect);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}