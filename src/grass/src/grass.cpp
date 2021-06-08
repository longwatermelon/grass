#include "grass.h"
#include "common.h"
#include "button.h"
#include "file_tree.h"
#include "text_entry.h"
#include "explorer.h"
#include "scrollbar.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <SDL_image.h>

#define BG_COLOR 30, 30, 30

namespace fs = std::filesystem;


Grass::Grass()
{
    SDL_Init(SDL_INIT_EVERYTHING);

    m_window = SDL_CreateWindow("Grass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
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
    /* constants */
    constexpr int scrollbar_width = 20;

    constexpr SDL_Rect main_text_dimensions = {
        300,
        40,
        1000 - 300 - scrollbar_width,
        800 - 40
    };

    /* Core ui elements that should not be touched */

    gui::common::Font font_textbox("res/CascadiaCode.ttf", 16);
    gui::common::Font font_tree("res/CascadiaCode.ttf", 14);

    std::vector<gui::TextEntry> text_entries;
    text_entries.emplace_back(gui::TextEntry(main_text_dimensions, { 50, 50, 50 }, gui::Cursor({ main_text_dimensions.x, main_text_dimensions.y }, { 255, 255, 255 }, font_textbox.char_dim()), gui::String(font_textbox.font(), { main_text_dimensions.x, main_text_dimensions.y }, "", font_textbox.char_dim(), { 255, 255, 255 })));

    std::vector<gui::Button> buttons;

    gui::Folder folder(".", gui::String(font_tree.font(), { 0, 60 }, "", font_tree.char_dim(), { 255, 255, 255 }), m_rend, true);
    gui::Tree tree(
        { 0, main_text_dimensions.y, main_text_dimensions.x, 800 - main_text_dimensions.y },
        folder,
        // when changing font size make sure to also change the 20 below to the y value of the char dimensions specified above
        { 0, main_text_dimensions.y, 200, font_tree.char_dim().y },
        m_rend
    );

    for (auto& f : tree.folder().folders())
    {
        f.collapse(m_rend);
    }

    tree.update_display();

    gui::Scrollbar scrollbar({
        main_text_dimensions.x + main_text_dimensions.w,
        main_text_dimensions.y,
        scrollbar_width,
        main_text_dimensions.h
    }, 0, 0, 0, { 40, 40, 40 }, { 100, 100, 100 });


    bool running = true;
    SDL_Event evt;

    /* Extra misc variables that should be added to */

    std::string current_open_fp;
    SDL_Texture* editor_image = nullptr;

    int prev_wx, prev_wy;
    SDL_GetWindowSize(m_window, &prev_wx, &prev_wy);


    /* Kb event variables and other very simple variables here */

    bool mouse_down = false;
    bool ctrl_down = false;

    while (running)
    {
        int mx, my;

        {
            // sdl_capturemouse is not good so here is a workaround
            int global_mx, global_my;
            SDL_GetGlobalMouseState(&global_mx, &global_my);

            int wx, wy;
            SDL_GetWindowPosition(m_window, &wx, &wy);

            mx = global_mx - wx;
            my = global_my - wy;
        }

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
                        m_selected_entry->mouse_down(mx, my);
                    }
                }

                if (!has_selected_item)
                {
                    m_selected_entry = nullptr;

                    for (auto& e : text_entries)
                    {
                        e.stop_highlight();
                    }
                }

                gui::File* file = tree.check_file_click(tree.folder(), mx, my);

                if (file)
                {
                    tree.set_selected_highlight_rect({
                        tree.rect().x,
                        file->rect().y,
                        tree.rect().w,
                        file->name().char_dim().y
                    });

                    if (tree.is_unsaved(current_open_fp))
                    {
                        std::ofstream ofs(current_open_fp + "~", std::ofstream::out | std::ofstream::trunc);
                        ofs << text_entries[0].text()->str();
                        ofs.close();
                    }

                    text_entries[0].stop_highlight();

                    current_open_fp = file->path();
                    std::string ext = fs::path(current_open_fp).extension().string();

                    if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".jpeg" || ext == ".webp")
                    {
                        editor_image = IMG_LoadTexture(m_rend, current_open_fp.c_str());

                        if (!editor_image)
                            editor_image = gui::common::render_text(m_rend, font_textbox.font(), SDL_GetError());

                        text_entries[0].text()->set_contents({ "" });
                        reset_entry_to_default(text_entries[0]);
                        text_entries[0].hide();
                        scrollbar.hide();
                    }
                    else
                    {
                        if (editor_image)
                            SDL_DestroyTexture(editor_image);

                        editor_image = nullptr;

                        text_entries[0].show();
                        scrollbar.show();
                        
                        if (fs::exists(current_open_fp + "~"))
                            load_file(current_open_fp + "~", text_entries[0]);
                        else
                            load_file(current_open_fp, text_entries[0]);
                    }

                    tree.update_display();

                    SDL_SetWindowTitle(m_window,
                        (std::string("Grass | Editing ") + file->name().str().c_str() + (tree.is_unsaved(file->path()) ? " - UNSAVED" : "")).c_str()
                    );
                }

                gui::Folder* folder = tree.check_folder_click(tree.folder(), mx, my);

                if (folder)
                {
                    tree.collapse_folder(*folder, m_rend);
                    tree.update_display();

                    tree.set_selected_highlight_rect({ 0, 0, 0, 0 });
                }

                scrollbar.check_clicked(mx, my);

            } break;

            case SDL_MOUSEBUTTONUP:
                mouse_down = false;

                if (m_selected_entry)
                {
                    m_selected_entry->mouse_up();
                }

                for (auto& btn : buttons)
                {
                    btn.set_down(false);
                }

                scrollbar.mouse_up();

                break;

            case SDL_TEXTINPUT:
                if (m_selected_entry)
                {
                    m_selected_entry->insert_char(evt.text.text[0]);
                    m_selected_entry->stop_highlight();
                    tree.append_unsaved_file(current_open_fp, m_window);
                }

                break;
            case SDL_KEYDOWN:
            {
                switch (evt.key.keysym.scancode)
                {
                case SDL_SCANCODE_RCTRL:
                case SDL_SCANCODE_LCTRL:
                    ctrl_down = true;
                    break;
                }

                switch (evt.key.keysym.sym)
                {
                case SDLK_s:
                    if (m_selected_entry)
                    {
                        if (ctrl_down && m_selected_entry == &text_entries[0])
                        {
                            std::ofstream ofs(current_open_fp, std::ofstream::out | std::ofstream::trunc);

                            for (auto& line : text_entries[0].text()->contents())
                            {
                                ofs << line << "\n";
                            }

                            ofs.close();

                            tree.erase_unsaved_file(current_open_fp, m_window);
                        }
                    }

                    break;
                case SDLK_d:
                    if (m_selected_entry)
                    {
                        if (ctrl_down && m_selected_entry == &text_entries[0])
                        {
                            tree.erase_unsaved_file(current_open_fp, m_window);
                            load_file(current_open_fp, text_entries[0]);
                        }
                    }

                    break;
                case SDLK_o:
                    if (ctrl_down)
                    {
                        SDL_Rect rect = { 0, 0, wx, wy };
                        SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 255);
                        SDL_RenderFillRect(m_rend, &rect);

                        std::string waiting_text = "Waiting for open folder dialog to finish";
                        SDL_Texture* text = gui::common::render_text(m_rend, font_textbox.font(), waiting_text.c_str());
                        rect.x = wx / 2 - waiting_text.size() * font_textbox.char_dim().x / 2;
                        rect.y = wy / 2 - font_textbox.char_dim().y / 2;

                        SDL_QueryTexture(text, nullptr, nullptr, &rect.w, &rect.h);
                        SDL_RenderCopy(m_rend, text, nullptr, &rect);

                        SDL_RenderPresent(m_rend);

                        // new window will take all input and releasing ctrl wont be detected in the main window
                        ctrl_down = false;

                        SDL_Point pos;
                        SDL_GetWindowPosition(m_window, &pos.x, &pos.y);
                        gui::Explorer e(tree.folder().path(), gui::ExplorerMode::DIR, pos);
                        std::string path = e.get_path();
                        
                        if (!path.empty())
                        {
                            tree.folder().change_directory(fs::absolute(path).string(), m_rend);
                            tree.update_display();
                            tree.set_selected_highlight_rect({ 0, 0, 0, 0 });

                            text_entries[0].text()->set_contents({ "" });
                            reset_entry_to_default(text_entries[0]);
                            
                            if (editor_image)
                                SDL_DestroyTexture(editor_image);

                            editor_image = 0;

                            tree.reset_default_rect();
                            tree.update_display();
                        }

                        SDL_DestroyTexture(text);
                    }

                    break;
                }

                if (m_selected_entry)
                {
                    switch (evt.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_RETURN:
                        m_selected_entry->insert_char('\n');
                        tree.append_unsaved_file(current_open_fp, m_window);
                        m_selected_entry->stop_highlight();
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        if (!mouse_down)
                        {
                            m_selected_entry->remove_char();
                            tree.append_unsaved_file(current_open_fp, m_window);
                        }
                        break;
                    }
                }

                if (m_selected_entry)
                {
                    SDL_Point movement{ 0, 0 };

                    if (evt.key.keysym.sym == SDLK_RIGHT)
                        movement.x = 1;

                    if (evt.key.keysym.sym == SDLK_LEFT)
                        movement.x = -1;

                    if (evt.key.keysym.sym == SDLK_UP)
                        movement.y = -1;

                    if (evt.key.keysym.sym == SDLK_DOWN)
                        movement.y = 1;

                    m_selected_entry->move_cursor_characters(movement.x, movement.y);

                    m_selected_entry->conditional_move_bounds_characters(
                        movement.x * m_selected_entry->move_bounds_by(),
                        movement.y * m_selected_entry->move_bounds_by()
                    );

                    m_selected_entry->conditional_jump_to_eol();
                }
            } break;
            case SDL_KEYUP:
            {
                switch (evt.key.keysym.scancode)
                {
                case SDL_SCANCODE_RCTRL:
                case SDL_SCANCODE_LCTRL:
                    ctrl_down = false;
                    break;
                }
            } break;
            case SDL_MOUSEWHEEL:
                if (gui::common::within_rect(tree.rect(), mx, my))
                {
                    tree.scroll(-evt.wheel.y, wy);
                }
                else if (gui::common::within_rect(text_entries[0].rect(), mx, my))
                {
                    text_entries[0].move_bounds_characters(0, -evt.wheel.y);
                }
                break;
            }
        }

        SDL_RenderClear(m_rend);

        if (mouse_down)
        {
            if (m_selected_entry)
            {
                m_selected_entry->move_cursor_to_click(mx, my);
            }
        }

        for (auto& btn : buttons)
        {
            btn.check_hover(mx, my);
            btn.render(m_rend);
        }

        tree.render(m_rend);

        if (gui::common::within_rect(tree.rect(), mx, my))
            tree.highlight_element(m_rend, mx, my);

        for (auto& e : text_entries)
        {
            if (e.hidden())
                continue;

            bool render_mouse = false;
            if (m_selected_entry == &e)
                render_mouse = true;

            e.render(m_rend, render_mouse);
        }


        if (!scrollbar.hidden())
        {
            SDL_Point min_bound = text_entries[0].min_bounds();
            SDL_Point max_bound = text_entries[0].max_bounds();

            if (!scrollbar.down())
            {
                scrollbar.set_bounds(min_bound.y, max_bound.y, text_entries[0].text()->contents().size() + (max_bound.y - min_bound.y));
            }
            else
            {
                scrollbar.move_with_cursor(my);

                text_entries[0].move_bounds_characters(0, scrollbar.min_position() - min_bound.y);
            }

            scrollbar.render(m_rend);
        }

        if (prev_wx != wx || prev_wy != wy)
        {
            text_entries[0].resize_to(wx - scrollbar_width, wy);
            tree.resize_to(wy);
            
            SDL_Rect entry_rect = text_entries[0].rect();
            scrollbar.move((entry_rect.x + entry_rect.w) - scrollbar.rect().x, 0);
            scrollbar.resize(wy);

            prev_wx = wx;
            prev_wy = wy;
        }

        if (editor_image)
        {
            int img_w, img_h;
            SDL_QueryTexture(editor_image, nullptr, nullptr, &img_w, &img_h);

            int img_x = (text_entries[0].rect().w / 2) - img_w / 2 + text_entries[0].rect().x;
            int img_y = (text_entries[0].rect().h / 2) - img_h / 2 + text_entries[0].rect().y;

            SDL_Rect dstrect = {
                img_x,
                img_y,
                img_w,
                img_h
            };

            SDL_RenderCopy(m_rend, editor_image, nullptr, &dstrect);
        }

        SDL_SetRenderDrawColor(m_rend, BG_COLOR, 255);
        SDL_RenderPresent(m_rend);
    }
    
    for (auto& path : tree.unsaved())
    {
        if (fs::exists(path + "~"))
            fs::remove(path + "~");
    }
}


void Grass::load_file(const std::string& fp, gui::TextEntry& entry)
{
    std::ifstream ifs(fp);

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(ifs, line)) lines.emplace_back(line);

    ifs.close();

    entry.text()->set_contents(lines);
    reset_entry_to_default(entry);
}


void Grass::reset_entry_to_default(gui::TextEntry& entry)
{
    entry.reset_bounds_x();
    entry.reset_bounds_y();
    entry.set_cursor_pos_characters(0, 0);
    entry.update_cache();
    entry.show();
}
