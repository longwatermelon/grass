#pragma once
#include "common.h"
#include <memory>


namespace gui
{
    class Text
    {
    public:
        Text(SDL_Renderer* rend, common::Font& font, SDL_Point pos, const std::string& text, SDL_Color color);

        void render();

        void set_text(const std::string& text);

        SDL_Rect rect() { return m_rect; }
        std::string text() { return m_text; }
        common::Font& font_ref() { return m_font; }

        void change_color(SDL_Color color) { m_color = color; }

    private:
        SDL_Rect m_rect;

        std::string m_text;
        common::Font& m_font;

        SDL_Color m_color;

        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_tex{ nullptr };

        // non owning, dont free
        SDL_Renderer* m_rend;
    };
}