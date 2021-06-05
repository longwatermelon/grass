#include "explorer.h"
#include "button.h"
#include <iostream>


gui::Explorer::Explorer(const std::string& path, ExplorerMode mode, SDL_Point pos)
    : m_current_path(path), m_mode(mode)
{
    m_window = SDL_CreateWindow((std::string("Select ") + (mode == ExplorerMode::FOLDER ? "folder" : "file")).c_str(), pos.x, pos.y, 600, 400, SDL_WINDOW_SHOWN);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);

    SDL_SetWindowGrab(m_window, SDL_TRUE);
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

    std::vector<Button*> buttons;
    buttons.emplace_back(new Button(m_rend, Text(font_button, button_pos, "Close", font_button_dim, { 255, 255, 255 }), { button_pos.x, button_pos.y, 95, 20 }, { 100, 100, 100 }, [&]() {
        running = false;
    }));

    while (running)
    {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                for (auto& btn : buttons)
                {
                    btn->check_clicked(mx, my);
                }
                break;
            }
        }

        SDL_RenderClear(m_rend);

        for (auto& btn : buttons)
        {
            if (btn)
            {
                btn->check_hover(mx, my);
                btn->render(m_rend);
            }
        }

        SDL_SetRenderDrawColor(m_rend, 50, 50, 50, 255);
        SDL_RenderPresent(m_rend);
        
        if (!running)
        {
            cleanup(buttons, &font_button);
            return;
        }
    }
}


void gui::Explorer::cleanup(std::vector<Button*>& buttons, TTF_Font** font)
{
    for (auto& btn : buttons)
    {
        delete btn;
        btn = 0;
    }

    buttons.clear();

    TTF_CloseFont(*font);
    *font = 0;

    SDL_SetWindowGrab(m_window, SDL_FALSE);

    SDL_DestroyRenderer(m_rend);
    SDL_DestroyWindow(m_window);
}