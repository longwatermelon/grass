#pragma once
#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

#define BG_COLOR 50, 50, 50


namespace gui::common
{
    struct TextureDeleter
    {
        void operator()(SDL_Texture* ptr)
        {
            if (ptr)
                SDL_DestroyTexture(ptr);

            ptr = 0;
        }
    };

    class Font
    {
    public:
        Font() = default;
        Font(const std::string& ttf_path, int pt_size, bool automatically_delete_font = true);
        ~Font();
        
        // closes current font and loads new font
        void load_font(const std::string& ttf_path, int pt_size);
        void change_pt_size(int size);

        void cleanup();

        TTF_Font* font() { return m_font; }
        TTF_Font** font_ptr() { return &m_font; }
        SDL_Point char_dim() { return m_char_dim; }
        SDL_Point& char_dim_ref() { return m_char_dim; }
        int pt_size() { return m_pt_size; }
        void automatically_delete(bool b) { m_delete_font = b; }

    private:
        TTF_Font* m_font{ nullptr };
        SDL_Point m_char_dim{ 0, 0 };

        std::string m_ttf_path;
        int m_pt_size{ 0 };

        bool m_delete_font{ true };
    };

    // nodiscard just to make sure i dont leak memory
    [[nodiscard]] SDL_Texture* render_text(SDL_Renderer* rend, TTF_Font* font, const char* text, SDL_Color color = { 255, 255, 255 });
    void center_rendered_text(SDL_Renderer* rend, SDL_Texture* tex, const char* text, SDL_Rect enclosing_rect, SDL_Point char_dim, SDL_Color color);

    bool within_rect(SDL_Rect rect, int x, int y);
}