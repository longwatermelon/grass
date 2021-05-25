#include "grass.h"
#include "common.h"
#include "button.h"
#include <iostream>
#include <SDL_ttf.h>


Grass::Grass()
{
    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow("Grass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);

    TTF_Init();
}


Grass::~Grass()
{
    SDL_DestroyRenderer(m_rend);
    SDL_DestroyWindow(m_window);

    SDL_Quit();
    TTF_Quit();
}


void Grass::mainloop()
{
    std::vector<gui::Button> buttons;
    buttons.emplace_back(gui::Button("test", { 100, 100, 1000, 50 }, { 100, 50, 103 }, []() { std::cout << "text\n"; }));
    buttons.emplace_back(gui::Button("sample text", { 50, 425, 300, 30 }, { 100, 100, 0 }, []() { std::cout << "sample text button\n"; }));

    TTF_Font* font_light = TTF_OpenFont("res/Montserrat-Light.ttf", 50);
    TTF_Font* font_medium = TTF_OpenFont("res/Montserrat-Medium.ttf", 50);
    TTF_Font* font_regular = TTF_OpenFont("res/Montserrat-Regular.ttf", 100);
    TTF_Font* font_thin = TTF_OpenFont("res/Montserrat-Thin.ttf", 50);

    bool running = true;
    SDL_Event evt;

    while (running)
    {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_MOUSEBUTTONDOWN:
                for (auto& btn : buttons)
                {
                    btn.check_clicked(mx, my);
                }
                break;
            case SDL_MOUSEBUTTONUP:
                for (auto& btn : buttons)
                {
                    btn.set_down(false);
                }
                break;
            }
        }

        SDL_RenderClear(m_rend);

        for (auto& btn : buttons)
        {
            btn.check_hover(mx, my);
            btn.render(m_rend, font_regular);
        }

        SDL_SetRenderDrawColor(m_rend, BG_COLOR, 255);

        SDL_RenderPresent(m_rend);
    }

    TTF_CloseFont(font_light);
    TTF_CloseFont(font_medium);
    TTF_CloseFont(font_regular);
    TTF_CloseFont(font_thin);
}