#include "explorer.h"
#include "button.h"
#include <iostream>


gui::Explorer::Explorer(const std::string& path, ExplorerMode mode)
    : m_current_path(path), m_mode(mode)
{
    m_window = SDL_CreateWindow("Open file dialog", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 600, 400, SDL_WINDOW_SHOWN);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);
}


void gui::Explorer::mainloop()
{
    bool running = true;
    SDL_Event evt;

    TTF_Font* font_button = TTF_OpenFont("res/CascadiaCode.ttf", 14);
    SDL_Point font_button_dim;
    TTF_SizeText(font_button, " ", &font_button_dim.x, &font_button_dim.y);
    
    SDL_Point window_size;
    SDL_GetWindowSize(m_window, &window_size.x, &window_size.y);

    SDL_Point button_pos = { window_size.x - 100, window_size.y - 25 };

    Button* save_button = new Button(m_rend, Text(font_button, button_pos, "Select", font_button_dim, { 255, 255, 255 }), { button_pos.x, button_pos.y, 95, 20 }, { 100, 100, 100 }, [&]() {
        running = false;
    });

    while (running)
    {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                save_button->check_clicked(mx, my);
                break;
            }
        }

        SDL_RenderClear(m_rend);

        if (save_button)
        {
            save_button->check_hover(mx, my);

            save_button->render(m_rend);
        }

        SDL_SetRenderDrawColor(m_rend, 10, 10, 10, 255);
        SDL_RenderPresent(m_rend);
        
        if (!running)
        {
            delete save_button;
            save_button = 0;
        }
    }

    SDL_DestroyRenderer(m_rend);
    SDL_DestroyWindow(m_window);

    TTF_CloseFont(font_button);
    font_button = nullptr;
}