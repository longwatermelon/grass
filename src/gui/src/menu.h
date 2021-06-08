#pragma once
#include "common.h"
#include "text.h"
#include <vector>
#include <string>
#include <memory>


namespace gui
{
    class Menu
    {
    public:
        Menu() = default;
        Menu(SDL_Point pos, int menu_width, const std::vector<std::string>& options, common::Font& font, SDL_Color bg_color, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, int mx, int my);

        void highlight_at_mouse(int mx, int my);

    private:
        SDL_Rect m_rect;

        std::vector<std::unique_ptr<Text>> m_options;
        common::Font& m_font;

        SDL_Color m_bg_color;
    };
}