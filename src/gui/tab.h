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
        Tab(std::unique_ptr<Text> text, SDL_Color color, const std::string& full_path, int rect_h);

        void render(SDL_Renderer* rend);

        void move(int x);

        int text_pixel_length();

        bool check_clicked(int mx, int my);
        void hover_highlight(int mx, int my);

        void set_clicked(bool b) { m_clicked = b; }
        void set_hover(bool b) { m_hover = b; }

        Text* text() { return m_text.get(); }
        std::string path() { return m_full_path; }
        SDL_Rect rect() { return m_rect; }

    private:
        SDL_Rect m_rect;
        std::unique_ptr<Text> m_text;
        std::string m_full_path;

        SDL_Color m_bg_color;

        bool m_hover{ false };
        bool m_clicked{ false };
    };
}
