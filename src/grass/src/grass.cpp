#include "grass.h"
#include "common.h"
#include "button.h"
#include "entry.h"
#include "file_tree.h"
#include <fstream>
#include <sstream>
#include <iostream>

#define BG_COLOR 30, 30, 30


Grass::Grass()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    m_window = SDL_CreateWindow("Grass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

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
    constexpr SDL_Rect main_text_dimensions = {
        300,
        40,
        1000 - 300,
        800 - 40
    };

    TTF_Font* font_regular = TTF_OpenFont("res/SourceCodePro-Regular.ttf", 100);

    std::vector<gui::TextEntry> text_entries;
    text_entries.emplace_back(gui::TextEntry(main_text_dimensions, gui::Text(font_regular, { 60, 60 }, "", { 10, 20 }, { 255, 255, 255 }), { 50, 50, 50 }, { 255, 255, 255 }));

    std::vector<gui::Button> buttons;

    gui::Tree tree(
        gui::Folder(".", gui::Text(font_regular, { 0, 60 }, "", { 10, 20 }, { 255, 255, 255 }), m_rend),
        // when changing font size make sure to also change the 20 below to the y value of the char dimensions specified above
        { 0, main_text_dimensions.y, 200, 20 },
        m_rend
    );

    for (auto& f : tree.folder().folders())
    {
        f.collapse();
    }

    tree.update_display();
    
    bool running = true;
    SDL_Event evt;

    int prev_wx, prev_wy;
    SDL_GetWindowSize(m_window, &prev_wx, &prev_wy);

    while (running)
    {
        int mx, my;
        SDL_GetMouseState(&mx, &my);

        int wx, wy;
        SDL_GetWindowSize(m_window, &wx, &wy);

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
                        m_selected_entry = &e;
                        has_selected_item = true;
                    }
                }

                if (!has_selected_item)
                    m_selected_entry = nullptr;

                gui::File* file = tree.check_file_click(tree.folder(), mx, my);

                if (file)
                {
                    std::string fp = file->path();
                    std::ifstream ifs(fp);

                    std::vector<std::string> lines;
                    std::string line;
                    while (std::getline(ifs, line)) lines.emplace_back(line);

                    ifs.close();

                    text_entries[0].text()->set_contents(lines);
                    text_entries[0].reset_bounds_x();
                    text_entries[0].reset_bounds_y();
                    text_entries[0].set_cursor_pos(0, 0);
                    text_entries[0].update_cache();

                    tree.update_display();
                }

                gui::Folder* folder = tree.check_folder_click(tree.folder(), mx, my);

                if (folder)
                {
                    tree.collapse_folder(*folder);
                    tree.update_display();
                }

            } break;

            case SDL_MOUSEBUTTONUP:
                for (auto& btn : buttons)
                {
                    btn.set_down(false);
                }
                break;

            case SDL_TEXTINPUT:
                if (m_selected_entry)
                {
                    m_selected_entry->add_char(evt.text.text[0]);
                }
                break;

            case SDL_KEYDOWN:
            {
                if (m_selected_entry)
                {
                    switch (evt.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_RETURN:
                        m_selected_entry->add_char('\n');
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        m_selected_entry->remove_char(1);
                        break;
                    case SDL_SCANCODE_DELETE:
                    {
                        SDL_Point coords = m_selected_entry->real_to_char_pos(m_selected_entry->real());

                        if (coords.y < m_selected_entry->text()->contents().size())
                            // coords.y == m_selected_entry->text()->contents().size() - 1 ? false : true
                            // if the user presses delete when the current line is the last element in contents, it 
                            // would delete the current line but not move up causing vector subscript out of range errors.
                            m_selected_entry->text()->erase(coords.x, coords.y, coords.y == m_selected_entry->text()->contents().size() - 1 ? false : true);
                    }
                    }
                }

                if (m_selected_entry)
                {
                    switch (evt.key.keysym.sym)
                    {
                    case SDLK_RIGHT:
                    {
                        gui::Text* t = m_selected_entry->text();
                        std::string line = t->get_line(m_selected_entry->real_to_char_pos(m_selected_entry->real()).y);
                        int cursor_pos = m_selected_entry->real_to_char_pos(m_selected_entry->real()).x;

                        if (cursor_pos < line.size())
                            m_selected_entry->move_cursor(1, 0);

                    } break;
                    case SDLK_LEFT:
                        m_selected_entry->move_cursor(-1, 0);
                        break;
                    case SDLK_UP:
                        m_selected_entry->move_cursor(0, -1);

                        if (m_selected_entry->get_current_line().size() <= m_selected_entry->get_coords().x)
                            m_selected_entry->jump_to_eol();
                        break;
                    case SDLK_DOWN:
                        m_selected_entry->move_cursor(0, 1);

                        if (m_selected_entry->get_current_line().size() <= m_selected_entry->get_coords().x)
                            m_selected_entry->jump_to_eol();
                        break;
                    case SDLK_TAB:
                    {
                        gui::Text* t = m_selected_entry->text();
                        SDL_Point coords = m_selected_entry->real_to_char_pos(m_selected_entry->real());

                        for (int i = 0; i < 4; ++i)
                        {
                            t->insert(coords.x, coords.y, ' ');
                            m_selected_entry->move_cursor(1, 0);
                        }
                    } break;
                }
                }
            } break;
            case SDL_MOUSEWHEEL:
                tree.scroll(-evt.wheel.y);
                break;
            }
        }

        SDL_RenderClear(m_rend);

        for (auto& btn : buttons)
        {
            btn.check_hover(mx, my);
            btn.render(m_rend);
        }

        tree.render(m_rend);

        for (auto& e : text_entries)
        {
            e.render(m_rend);
        }

        if (m_selected_entry)
        {
            m_selected_entry->draw_cursor(m_rend);
        }

        if (prev_wx != wx || prev_wy != wy)
        {
            text_entries[0].resize_to(wx, wy);
            prev_wx = wx;
            prev_wy = wy;
        }

        SDL_SetRenderDrawColor(m_rend, BG_COLOR, 255);

        SDL_RenderPresent(m_rend);
    }

    TTF_CloseFont(font_regular);
}
