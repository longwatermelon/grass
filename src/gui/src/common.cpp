#include "common.h"


void gui::common::draw_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Rect rect, SDL_Color color)
{
    if (!text[0])
        return;

    SDL_Surface* surf = TTF_RenderText_Solid(font, text, color);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_RenderCopy(rend, tex, nullptr, &rect);

    SDL_FreeSurface(surf);
    SDL_DestroyTexture(tex);
}


void gui::common::draw_centered_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Point pos, SDL_Rect enclosing_rect, SDL_Point char_dim, SDL_Color col)
{
    int text_len = strlen(text) * char_dim.x;
    int text_height = char_dim.y;

    int x_offset = (enclosing_rect.w - text_len) / 2;
    int y_offset = (enclosing_rect.h - text_height) / 2;

    draw_text(rend, font, text, { pos.x + x_offset, pos.y + y_offset, text_len, text_height }, col);
}


SDL_Texture* gui::common::render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color)
{
    if (!text[0])
        return nullptr;

    SDL_Surface* surf = TTF_RenderText_Blended(font, text, color);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_FreeSurface(surf);

    return tex;
}


bool gui::common::within_rect(SDL_Rect rect, int x, int y)
{
    return x > rect.x && x < rect.x + rect.w
        && y > rect.y && y < rect.y + rect.h;
}