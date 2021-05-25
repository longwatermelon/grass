#include "grass.h"
#include "common.h"
#include "button.h"
#include "entry.h"
#include <iostream>
#include <any>


Grass::Grass()
{
    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow("Grass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 1000, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);

    TTF_Init();

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
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
    TTF_Font* font_regular = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 50);

    std::vector<gui::Button> buttons;
    buttons.emplace_back(gui::Button(gui::Text(font_regular, { 100, 100 }, "test", { 10, 20 }, { 255, 255, 255 }), { 100, 100, 1000, 50 }, { 100, 50, 103 }, []() { std::cout << "text\n"; }));
    buttons.emplace_back(gui::Button(gui::Text(font_regular, { 50, 425 }, "sample text", { 10, 20 }, { 255, 255, 255 }), { 50, 425, 300, 30 }, { 100, 100, 0 }, []() { std::cout << "sample text button\n"; }));

    std::vector<gui::TextEntry> text_entries;
    text_entries.emplace_back(gui::TextEntry(SDL_Rect{ 100, 600, 700, 200 }, 1, gui::Text(font_regular, { 100, 600 }, "", { 10, 20 }, { 0, 0, 0 })));

    gui::TextEntry* selected_entry{ nullptr };
    
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
            {
                for (auto& btn : buttons)
                {
                    btn.check_clicked(mx, my);
                }

                bool has_selected_item{ false };

                for (auto& e : text_entries)
                {
                    if (e.check_clicked(mx, my))
                    {
                        selected_entry = &e;
                        has_selected_item = true;
                    }
                }

                if (!has_selected_item)
                    selected_entry = nullptr;

            } break;

            case SDL_MOUSEBUTTONUP:
                for (auto& btn : buttons)
                {
                    btn.set_down(false);
                }
                break;

            case SDL_TEXTINPUT:
                if (selected_entry)
                {
                    selected_entry->add_char(evt.text.text[0]);
                }
                break;

            case SDL_KEYDOWN:
            {
                if (selected_entry)
                {
                    switch (evt.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_RETURN:
                        selected_entry->add_char('\n');
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        selected_entry->remove_char(1);
                        break;
                    }
                }
            } break;
            }
        }

        SDL_RenderClear(m_rend);

        for (auto& btn : buttons)
        {
            btn.check_hover(mx, my);
            btn.render(m_rend);
        }

        for (auto& e : text_entries)
        {
            e.render(m_rend);
        }

        if (selected_entry)
        {
            selected_entry->draw_cursor(m_rend);
        }

        SDL_SetRenderDrawColor(m_rend, BG_COLOR, 255);

        SDL_RenderPresent(m_rend);
    }

    TTF_CloseFont(font_regular);
}