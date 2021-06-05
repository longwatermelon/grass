#include "common.h"


[[nodiscard]] SDL_Texture* gui::common::render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color)
{
    if (!text[0])
        return nullptr;

    SDL_Surface* surf = TTF_RenderText_Blended(font, text, color);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");
    SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surf);

    SDL_FreeSurface(surf);

    return tex;
}


void gui::common::center_rendered_text(SDL_Renderer* rend, SDL_Texture* tex, const char* text, SDL_Rect enclosing_rect, SDL_Point char_dim, SDL_Color color)
{
    int text_len = strlen(text) * char_dim.x;
    int text_height = char_dim.y;

    int x_offset = (enclosing_rect.w - text_len) / 2;
    int y_offset = (enclosing_rect.h - text_height) / 2;

    SDL_Rect rect = {
        enclosing_rect.x + x_offset,
        enclosing_rect.y + y_offset,
        text_len,
        text_height
    };

    if (tex)
        SDL_RenderCopy(rend, tex, nullptr, &rect);
}


bool gui::common::within_rect(SDL_Rect rect, int x, int y)
{
    return x > rect.x && x < rect.x + rect.w
        && y > rect.y && y < rect.y + rect.h;
}