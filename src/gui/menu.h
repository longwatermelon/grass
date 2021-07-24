#pragma once
#include "common.h"
#include "text.h"
#include <vector>
#include <string>
#include <memory>
#include <functional>


namespace gui
{
    struct MenuElement
    {
        std::string text;
        std::function<void(void)> function;
    };

    class Menu
    {
    public:
        Menu() = default;
        Menu(SDL_Point pos, int menu_width, const std::vector<MenuElement>& options, common::Font& font, SDL_Color bg_color, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, int mx, int my);

        // returns index of menu element at (mx, my)
        int mouse_index(int mx, int my);
        // returns true if clicked, automatically executes function of the element that was clicked
        bool check_clicked(int mx, int my);

    private:
        SDL_Rect m_rect;

        std::vector<std::unique_ptr<Text>> m_options;
        std::vector<std::function<void(void)>> m_functions;

        common::Font& m_font;

        SDL_Color m_bg_color;
    };
}