#include "grass.h"
#include "common.h"
#include "button.h"
#include "entry.h"
#include "file_tree.h"
#include "text_entry.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <SDL_image.h>

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
    IMG_Init(IMG_INIT_PNG);
}


Grass::~Grass()
{
    SDL_DestroyRenderer(m_rend);
    SDL_DestroyWindow(m_window);

    SDL_Quit();
    TTF_Quit();
    IMG_Quit();
}


void Grass::mainloop()
{
    constexpr SDL_Rect main_text_dimensions = {
        300,
        40,
        1000 - 300,
        800 - 40
    };

    SDL_Rect dstrect;

    TTF_Font* font_regular = TTF_OpenFont("res/CascadiaCode.ttf", 36);
    SDL_Texture* img = nullptr;
    int img_x, img_y, img_w, img_h;

    std::vector<gui::TextEntry> text_entries;
    //text_entries.emplace_back(gui::TextEntry(main_text_dimensions, gui::Text(font_regular, { 60, 60 }, "", { 9, 18 }, { 255, 255, 255 }), { 50, 50, 50 }, { 255, 255, 255 }));
    text_entries.emplace_back(gui::TextEntry(main_text_dimensions, { 50, 50, 50 }, gui::Cursor({ main_text_dimensions.x, main_text_dimensions.y }, { 255, 255, 255 }, { 9, 18 }), gui::Text(font_regular, { main_text_dimensions.x, main_text_dimensions.y }, "", { 9, 18 }, { 255, 255, 255 })));

    std::vector<gui::Button> buttons;

    gui::Folder folder(".", gui::Text(font_regular, { 0, 60 }, "", { 8, 16 }, { 255, 255, 255 }), m_rend, true);
    gui::Tree tree(
        folder,
        // when changing font size make sure to also change the 20 below to the y value of the char dimensions specified above
        { 0, main_text_dimensions.y, 200, 16 },
        m_rend
    );

    for (auto& f : tree.folder().folders())
    {
        f.collapse(m_rend);
    }

    tree.update_display();
    
    bool running = true;
    SDL_Event evt;

    int prev_wx, prev_wy;
    SDL_GetWindowSize(m_window, &prev_wx, &prev_wy);

    bool mouse_down = false;

    while (running)
    {
        

        SDL_CaptureMouse(SDL_TRUE);

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
                        /*m_selected_entry->move_cursor_to_click(mx, my);
                        m_selected_entry->start_highlight();*/
                    }
                }

                if (!has_selected_item)
                    m_selected_entry = nullptr;
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
                    m_selected_entry->insert_char(evt.text.text[0]);
                    //m_selected_entry->stop_highlight();
                }
                break;
            case SDL_KEYDOWN:
            {
                if (m_selected_entry)
                {
                    switch (evt.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_RETURN:
                        m_selected_entry->insert_char('\n');
                        //m_selected_entry->stop_highlight();
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        if (!mouse_down)
                            m_selected_entry->remove_char();
                        break;
                    }
                }

                if (m_selected_entry)
                {
                    switch (evt.key.keysym.sym)
                    {
                    case SDLK_RIGHT:
                    {
                        gui::Text* t = m_selected_entry->text();
                        /*std::string line = t->get_line(m_selected_entry->real_to_char_pos(m_selected_entry->real()).y);
                        int cursor_pos = m_selected_entry->real_to_char_pos(m_selected_entry->real()).x;*/

                        m_selected_entry->move_cursor_characters(1, 0);

                        /*if (cursor_pos < line.size())
                            m_selected_entry->move_cursor_characters(1, 0);*/

                        //m_selected_entry->stop_highlight();
                    } break;
                    case SDLK_LEFT:
                        m_selected_entry->move_cursor_characters(-1, 0);
                        //m_selected_entry->stop_highlight();
                        break;
                    case SDLK_UP:
                        m_selected_entry->move_cursor_characters(0, -1);

                        /*if (m_selected_entry->get_current_line().size() <= m_selected_entry->get_coords().x)
                            m_selected_entry->jump_to_eol();*/

                        //m_selected_entry->stop_highlight();
                        break;
                    case SDLK_DOWN:
                        m_selected_entry->move_cursor_characters(0, 1);

                        /*if (m_selected_entry->get_current_line().size() <= m_selected_entry->get_coords().x)
                            m_selected_entry->jump_to_eol();*/

                        //m_selected_entry->stop_highlight();
                        break;
                    case SDLK_TAB:
                    {
                        gui::Text* t = m_selected_entry->text();
                        //SDL_Point coords = m_selected_entry->real_to_char_pos(m_selected_entry->real());

                        /*for (int i = 0; i < 4; ++i)
                        {
                            t->insert(coords.x, coords.y, ' ');
                            m_selected_entry->move_cursor_characters(1, 0);
                        }*/
                    } break;
                    }
                }
            } break;
            }
            
        }

#if 0
        while (SDL_PollEvent(&evt))
        {
            switch (evt.type)
            {
            case SDL_QUIT:
                running = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
            {
                mouse_down = true;

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
                        m_selected_entry->move_cursor_to_click(mx, my);
                        m_selected_entry->start_highlight();
                    }
                }

                if (!has_selected_item)
                    m_selected_entry = nullptr;

                gui::File* file = tree.check_file_click(tree.folder(), mx, my);

                if (file)
                {
                    std::string fp = file->path();
                    if (fp.substr(fp.length() - 4, fp.length()) == ".png")
                    {
                        img = IMG_LoadTexture(m_rend, fp.c_str());
                        text_entries[0].text()->set_contents({ "" });
                    }

                    else
                    {
                        SDL_DestroyTexture(img);
                        img = nullptr;
                        std::ifstream ifs(fp);

                        std::vector<std::string> lines;
                        std::string line;
                        while (std::getline(ifs, line)) lines.emplace_back(line);

                        ifs.close();

                        text_entries[0].text()->set_contents(lines);

                    }

                    text_entries[0].reset_bounds_x();
                    text_entries[0].reset_bounds_y();
                    text_entries[0].set_cursor_pos(0, 0);
                    text_entries[0].update_cache();
                    
                    tree.update_display();

                    SDL_SetWindowTitle(m_window, (std::string("Grass | Editing ") + file->name().str().c_str()).c_str());

                    text_entries[0].stop_highlight();
                }

                gui::Folder* folder = tree.check_folder_click(tree.folder(), mx, my);

                if (folder)
                {
                    tree.collapse_folder(*folder, m_rend);
                    tree.update_display();
                }

            } break;

            case SDL_MOUSEBUTTONUP:
                mouse_down = false;

                if (m_selected_entry)
                {
                    m_selected_entry->stop_highlight_if_not_highlight();
                    m_selected_entry->mouse_up();
                    m_selected_entry->bound_cursor_to_box();
                }

                for (auto& btn : buttons)
                {
                    btn.set_down(false);
                }
                break;

            case SDL_TEXTINPUT:
                if (m_selected_entry)
                {
                    m_selected_entry->add_char(evt.text.text[0]);
                    m_selected_entry->stop_highlight();
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
                        m_selected_entry->stop_highlight();
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        if (!mouse_down)
                            m_selected_entry->remove_char(1);
                        break;
                    case SDL_SCANCODE_DELETE:
                    {
                        m_selected_entry->stop_highlight();
                        SDL_Point coords = m_selected_entry->real_to_char_pos(m_selected_entry->real());

                        if (coords.y < m_selected_entry->text()->contents().size())
                        {
                            // if the user presses delete when the current line is the last element in contents, it 
                            // would delete the current line but not move up causing vector subscript out of range errors.
                            m_selected_entry->text()->erase(coords.x, coords.y, m_selected_entry->text()->contents().size() == 0 ? false : true);
                            m_selected_entry->clear_cache();
                        }
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

                        m_selected_entry->stop_highlight();
                    } break;
                    case SDLK_LEFT:
                        m_selected_entry->move_cursor(-1, 0);
                        m_selected_entry->stop_highlight();
                        break;
                    case SDLK_UP:
                        m_selected_entry->move_cursor(0, -1);

                        if (m_selected_entry->get_current_line().size() <= m_selected_entry->get_coords().x)
                            m_selected_entry->jump_to_eol();

                        m_selected_entry->stop_highlight();
                        break;
                    case SDLK_DOWN:
                        m_selected_entry->move_cursor(0, 1);

                        if (m_selected_entry->get_current_line().size() <= m_selected_entry->get_coords().x)
                            m_selected_entry->jump_to_eol();

                        m_selected_entry->stop_highlight();
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
                if (mx > 0 && mx < main_text_dimensions.x)
                    tree.scroll(-evt.wheel.y, wy);
                else
                    text_entries[0].scroll(-evt.wheel.y);
                break;
            }
        }
#endif

        SDL_RenderClear(m_rend);

        if (mouse_down)
        {
            if (m_selected_entry)
            {
                //m_selected_entry->move_cursor_to_click(mx, my);
            }
        }

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

        if (prev_wx != wx || prev_wy != wy)
        {
            //text_entries[0].resize_to(wx, wy);
            prev_wx = wx;
            prev_wy = wy;
        }

        SDL_SetRenderDrawColor(m_rend, BG_COLOR, 255);

        if (img)
        {
            SDL_QueryTexture(img, nullptr, nullptr, &img_w, &img_h);
            img_x = (text_entries[0].rect().w / 2) - img_w/2 + text_entries[0].rect().x;
            img_y = (text_entries[0].rect().h / 2) - img_h/2 + text_entries[0].rect().y;
            dstrect = {
                img_x,
                img_y,
                img_w,
                img_h
            };
            SDL_RenderCopy(m_rend, img, nullptr, &dstrect);
        }
        SDL_RenderPresent(m_rend);
    }

    TTF_CloseFont(font_regular);
}
