#pragma once
#include "text.h"
#include <SDL.h>
#include <string>
#include <memory>


namespace gui
{
    class Tab
    {
    public:
        Tab(std::unique_ptr<Text> text, SDL_Color color, const std::string& full_path);

        void render(SDL_Renderer* rend);

        int text_pixel_length();

        bool check_clicked(int mx, int my);

        Text* text() { return m_text.get(); }
        std::string path() { return m_full_path; }

    private:
        std::unique_ptr<Text> m_text;
        std::string m_full_path;

        SDL_Color m_bg_color;
    };
}
