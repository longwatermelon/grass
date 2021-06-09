#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#define BG_COLOR 30, 30, 30


namespace gui::common
{
    struct TextureDeleter
    {
        void operator()(SDL_Texture* ptr)
        {
            if (ptr)
                SDL_DestroyTexture(ptr);
        }
    };

    class Font
    {
    public:
        Font() = default;
        Font(const std::string& ttf_path, int pt_size);
        ~Font();
        
        // closes current font and loads new font
        void load_font(const std::string& ttf_path, int pt_size);

        TTF_Font* font() { return m_font; }
        TTF_Font** font_ptr() { return &m_font; }
        SDL_Point char_dim() { return m_char_dim; }

    private:
        TTF_Font* m_font;
        SDL_Point m_char_dim;
    };

    // nodiscard just to make sure i dont leak memory
    [[nodiscard]] SDL_Texture* render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color = { 255, 255, 255 });
    void center_rendered_text(SDL_Renderer* rend, SDL_Texture* tex, const char* text, SDL_Rect enclosing_rect, SDL_Point char_dim, SDL_Color color);

    bool within_rect(SDL_Rect rect, int x, int y);
}