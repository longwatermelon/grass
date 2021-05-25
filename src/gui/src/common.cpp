#include "common.h"


void gui::common::draw_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color)
{
    SDL_Surface* surf = TTF_RenderText_Blended(font, text, color);
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_RenderCopy(rend, tex, nullptr, &rect);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}


bool gui::common::within_rect(SDL_Rect rect, int x, int y)
{
    return x > rect.x && x < rect.x + rect.w
        && y > rect.y && y < rect.y + rect.h;
}