#include "grass.h"
#include "common.h"
#include "button.h"
#include "entry.h"
#include <iostream>
#include <any>
#include <fstream>


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

    std::vector<gui::TextEntry> text_entries;
    text_entries.emplace_back(gui::TextEntry(SDL_Rect{ 0, 40, 1000, 960 }, gui::Text(font_regular, { 0, 40 }, "", { 10, 20 }, { 0, 0, 0 })));
    text_entries.emplace_back(gui::TextEntry(SDL_Rect{ 0, 0, 400, 20 }, gui::Text(font_regular, { 0, 0 }, "", { 10, 20 }, { 0, 0, 0 })));

    std::vector<gui::Button> buttons;
    buttons.emplace_back(gui::Button(gui::Text(font_regular, { 420, 0 }, "Save", { 10, 20 }, { 255, 255, 255 }), { 420, 0, 100, 20 }, { 0, 150, 0 }, [&]() {
        std::string fp = text_entries[1].text()->str();
        std::ofstream ofs(fp);
        
        for (auto& s : text_entries[0].text()->contents())
        {
            ofs << s << "\n";
        }

        ofs.close();
    }));

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
                    case SDL_SCANCODE_DELETE:
                    {
                        SDL_Point coords = selected_entry->real_to_char_pos(selected_entry->real());

                        if (coords.y < selected_entry->text()->contents().size())
                            // coords.y == selected_entry->text()->contents().size() - 1 ? false : true
                            // if the user presses delete when the current line is the last element in contents, it 
                            // would delete the current line but not move up causing vector subscript out of range errors.
                            selected_entry->text()->erase(coords.x, coords.y, coords.y == selected_entry->text()->contents().size() - 1 ? false : true);
                    }
                    }
                }

                switch (evt.key.keysym.sym)
                {
                case SDLK_RIGHT:
                {
                    gui::Text* t = selected_entry->text();
                    std::string line = t->get_line(selected_entry->real_to_char_pos(selected_entry->real()).y);
                    int cursor_pos = selected_entry->real_to_char_pos(selected_entry->real()).x;

                    if (cursor_pos < line.size())
                        selected_entry->move_cursor(1, 0);
                    
                } break;
                case SDLK_LEFT:
                    selected_entry->move_cursor(-1, 0);
                    break;
                case SDLK_UP:
                    selected_entry->move_cursor(0, -1);
                    selected_entry->jump_to_eol();
                    break;
                case SDLK_DOWN:
                    selected_entry->move_cursor(0, 1);
                    selected_entry->jump_to_eol();
                    break;
                case SDLK_TAB:
                {
                    gui::Text* t = selected_entry->text();
                    SDL_Point coords = selected_entry->real_to_char_pos(selected_entry->real());

                    for (int i = 0; i < 4; ++i)
                    {
                        t->insert(coords.x, coords.y, ' ');
                        selected_entry->move_cursor(1, 0);
                    }
                } break;
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