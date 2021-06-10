#include "grass.h"
#include "common.h"
#include "button.h"
#include "file_tree.h"
#include "text_entry.h"
#include "explorer.h"
#include "scrollbar.h"
#include "menu.h"
#include "basic_text_entry.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <memory>
#include <SDL_image.h>

namespace fs = std::filesystem;


Grass::Grass(const std::string& exe_dir)
    : m_exe_dir(exe_dir + '/')
{
    SDL_Init(SDL_INIT_EVERYTHING);

    m_window = SDL_CreateWindow("Grass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    m_font_textbox.load_font("res/CascadiaCode.ttf", 16);
    m_font_tree.load_font("res/CascadiaCode.ttf", 14);

    m_font_textbox.automatically_delete(false);
    m_font_tree.automatically_delete(false);
}


Grass::~Grass()
{
    SDL_DestroyRenderer(m_rend);
    SDL_DestroyWindow(m_window);

    SDL_Quit();
    IMG_Quit();
    TTF_Quit();
}


void Grass::mainloop()
{
    /* constants */
    constexpr int scrollbar_width = 20;

    constexpr SDL_Rect main_text_dimensions = {
        340,
        40,
        1000 - 340 - scrollbar_width,
        800 - 40
    };

    /* Core ui elements that should not be touched */
    m_text_entries.emplace_back(gui::TextEntry(main_text_dimensions, { 30, 30, 30 }, gui::Cursor({ main_text_dimensions.x, main_text_dimensions.y }, { 255, 255, 255 }, m_font_textbox.char_dim()), gui::String(m_font_textbox.font(), { main_text_dimensions.x, main_text_dimensions.y }, "", m_font_textbox.char_dim(), { 255, 255, 255 })));

    std::vector<std::unique_ptr<gui::Button>> buttons;

    gui::Folder folder(fs::absolute(".").string(), gui::String(m_font_tree.font(), { 0, 60 }, "", m_font_tree.char_dim(), { 255, 255, 255 }), m_rend, true);

    m_tree = new gui::Tree(
        { 0, main_text_dimensions.y, main_text_dimensions.x, 800 - main_text_dimensions.y },
        folder,
        // when changing font size make sure to also change the 20 below to the y value of the char dimensions specified above
        { 0, main_text_dimensions.y, 200, m_font_tree.char_dim().y },
        m_rend,
        m_exe_dir
    );

    m_tree->update_display();

    m_scrollbar = gui::Scrollbar({
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

    gui::Menu* menu = 0;

    m_basic_text_entries.emplace_back(gui::BasicTextEntry({ 300, 20, 100, m_font_textbox.char_dim().y },
        gui::Cursor({ 300, 20 }, { 255, 255, 255 }, m_font_textbox.char_dim()), 
        std::make_unique<gui::Text>(gui::Text(m_rend, m_font_textbox, { 300, 20 }, "", { 255, 255, 255 })),
        { 30, 30, 30 }
    ));

    m_selected_basic_entry = &m_basic_text_entries[0];

    // put the buttons here so they have access to all the previous variables
    buttons.emplace_back(new gui::Button(m_rend, gui::String(m_font_tree.font(), { 0, 0 }, "Help", m_font_tree.char_dim(), { 255, 255, 255 }), { 0, 0, 60, 20 }, { 70, 70, 70 }, [&]() {
        if (editor_image)
            SDL_DestroyTexture(editor_image);

        editor_image = 0;

        load_file(m_exe_dir + "res/help.txt", m_text_entries[0]);
    }));

    /* Kb event variables */

    bool mouse_down = false;
    bool ctrl_down = false;
    bool shift_down = false;

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
                if (evt.button.button == SDL_BUTTON_LEFT && !mouse_down)
                {
                    mouse_down = true;
                    bool clicked = false;

                    if (menu)
                    {
                        menu->check_clicked(mx, my);

                        delete menu;
                        menu = 0;
                        clicked = true;
                    }

                    if (!clicked)
                    {
                        for (auto& btn : buttons)
                        {
                            if (btn->check_clicked(mx, my))
                            {
                                clicked = true;
                                break;
                            }
                        }
                    }

                    if (clicked)
                    {
                        m_selected_entry = 0;

                        for (auto& e : m_text_entries)
                            e.stop_highlight();
                    }
                    else
                    {
                        for (auto& e : m_text_entries)
                        {
                            if (e.check_clicked(mx, my))
                            {
                                m_selected_entry = &e;
                                clicked = true;
                                m_selected_entry->mouse_down(mx, my);
                            }
                        }
                    }
                    
                    if (clicked)
                    {
                        for (auto& e : m_basic_text_entries)
                        {
                            e.set_cursor_visible(false);
                        }

                        m_selected_basic_entry = 0;
                    }
                    else
                    {
                        for (auto& e : m_basic_text_entries)
                        {
                            if (e.check_clicked(mx, my))
                            {
                                clicked = true;
                                m_selected_basic_entry = &e;
                                m_selected_basic_entry->set_cursor_visible(true);
                            }
                        }
                    }
                    

                    if (clicked)
                    {
                        m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                    }
                    else
                    {
                        gui::File* file = m_tree->check_file_click(m_tree->folder(), mx, my);

                        if (file)
                        {
                            if (!fs::exists(file->path()))
                            {
                                SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Error", std::string("'" + file->path() + "' doesn't exist").c_str(), m_window);
                                break;
                            }

                            m_tree->set_selected_highlight_rect({
                                m_tree->rect().x,
                                file->rect().y,
                                m_tree->rect().w,
                                file->name().char_dim().y
                                });

                            if (m_tree->is_unsaved(current_open_fp))
                            {
                                std::ofstream ofs(current_open_fp + "~", std::ofstream::out | std::ofstream::trunc);
                                ofs << m_text_entries[0].text()->str();
                                ofs.close();
                            }

                            m_text_entries[0].stop_highlight();

                            current_open_fp = file->path();
                            std::string ext = fs::path(current_open_fp).extension().string();

                            // try and cover all common image file extensions even if they are not supported
                            if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".jpeg" || ext == ".webp")
                            {
                                editor_image = IMG_LoadTexture(m_rend, current_open_fp.c_str());

                                if (!editor_image)
                                    editor_image = gui::common::render_text(m_rend, m_font_textbox.font(), SDL_GetError());

                                m_text_entries[0].text()->set_contents({ "" });
                                reset_entry_to_default(m_text_entries[0]);
                                m_text_entries[0].hide();
                                m_scrollbar.hide();
                            }
                            else
                            {
                                if (editor_image)
                                    SDL_DestroyTexture(editor_image);

                                editor_image = nullptr;

                                m_text_entries[0].show();
                                m_scrollbar.show();

                                if (fs::exists(current_open_fp + "~"))
                                    load_file(current_open_fp + "~", m_text_entries[0]);
                                else
                                    load_file(current_open_fp, m_text_entries[0]);
                            }

                            m_tree->update_display();

                            SDL_SetWindowTitle(m_window,
                                (std::string("Grass | Editing ") + file->name().str().c_str() + (m_tree->is_unsaved(file->path()) ? " - UNSAVED" : "")).c_str()
                            );

                            break;
                        }

                        gui::Folder* folder = m_tree->check_folder_click(m_tree->folder(), mx, my);

                        if (folder)
                        {
                            m_tree->collapse_folder(*folder, m_rend);
                            m_tree->update_display();

                            m_tree->set_selected_highlight_rect({
                                m_tree->rect().x,
                                folder->rect().y,
                                m_tree->rect().w,
                                folder->name().char_dim().y
                            });

                            break;
                        }

                        if (folder || file)
                            clicked = true;
                    }

                    if (!clicked)
                        m_scrollbar.check_clicked(mx, my);
                }

                if (evt.button.button == SDL_BUTTON_RIGHT)
                {
                    gui::Folder* f = m_tree->check_folder_click(m_tree->folder(), mx, my);

                    if (f)
                    {
                        menu = new gui::Menu({ mx, my }, 100, {
                            {"New file", [&]() {
                                f->create_new_file("New file");
                                f->load(m_rend);
                                m_tree->update_display();
                                m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                            }}
                        }, m_font_tree, { 40, 40, 40 }, m_rend);
                    }

                    gui::File* file = m_tree->check_file_click(m_tree->folder(), mx, my);

                    if (file)
                    {
                        menu = new gui::Menu({ mx, my }, 100, {
                            {"Delete file", [&]() {
                                if (current_open_fp == file->path())
                                {
                                    reset_entry_to_default(m_text_entries[0]);
                                    m_text_entries[0].text()->set_contents({ "" });

                                    m_tree->erase_unsaved_file(file->path(), m_window);

                                    if (fs::exists(file->path() + '~'))
                                        fs::remove(file->path() + '~');
                                }

                                file->delete_self();
                                m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                                m_tree->reload_outdated_folders(m_rend, true);
                            }}
                        }, m_font_tree, { 40, 40, 40 }, m_rend);
                    }
                }
            } break;

            case SDL_MOUSEBUTTONUP:
                mouse_down = false;

                if (m_selected_entry)
                {
                    m_selected_entry->mouse_up();
                }

                for (auto& btn : buttons)
                {
                    btn->set_down(false);
                }

                m_scrollbar.mouse_up();

                break;

            case SDL_TEXTINPUT:
                if (m_selected_entry)
                {
                    if (m_selected_entry->mode() == gui::EntryMode::HIGHLIGHT)
                        m_selected_entry->erase_highlighted_section();

                    m_selected_entry->insert_char(evt.text.text[0]);
                    m_tree->append_unsaved_file(current_open_fp, m_window);
                }

                if (m_selected_basic_entry)
                {
                    m_selected_basic_entry->add_char(evt.text.text[0]);
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
                        if (ctrl_down && m_selected_entry == &m_text_entries[0])
                        {
                            std::ofstream ofs(current_open_fp, std::ofstream::out | std::ofstream::trunc);

                            for (auto& line : m_text_entries[0].text()->contents())
                            {
                                ofs << line << "\n";
                            }

                            ofs.close();

                            m_tree->erase_unsaved_file(current_open_fp, m_window);
                        }
                    }

                    break;
                case SDLK_d:
                    if (m_selected_entry)
                    {
                        if (ctrl_down && m_selected_entry == &m_text_entries[0])
                        {
                            m_tree->erase_unsaved_file(current_open_fp, m_window);
                            load_file(current_open_fp, m_text_entries[0]);
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
                        SDL_Texture* text = gui::common::render_text(m_rend, m_font_textbox.font(), waiting_text.c_str());
                        rect.x = wx / 2 - waiting_text.size() * m_font_textbox.char_dim().x / 2;
                        rect.y = wy / 2 - m_font_textbox.char_dim().y / 2;

                        SDL_QueryTexture(text, nullptr, nullptr, &rect.w, &rect.h);
                        SDL_RenderCopy(m_rend, text, nullptr, &rect);

                        SDL_RenderPresent(m_rend);

                        // new window will take all input and releasing ctrl wont be detected in the main window
                        ctrl_down = false;

                        SDL_Point pos;
                        SDL_GetWindowPosition(m_window, &pos.x, &pos.y);
                        gui::common::Font explorer_font(m_exe_dir + "res/CascadiaCode.ttf", 14);

                        gui::Explorer e(m_tree->folder().path(), gui::ExplorerMode::DIR, pos, m_exe_dir, explorer_font);
                        std::string path = e.get_path();

                        // sdl_destroyrenderer takes too much time sometimes but it seems instantaneous if the window is hidden before cleanup and then it cleans up
                        // on a separate thread
                        std::thread thr_cleanup(&gui::Explorer::cleanup_window, &e);
                        thr_cleanup.detach();
                        
                        if (!path.empty())
                        {
                            m_tree->folder().change_directory(fs::absolute(path).string(), m_rend);
                            m_tree->update_display();
                            m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });

                            m_text_entries[0].text()->set_contents({ "" });
                            reset_entry_to_default(m_text_entries[0]);
                            
                            if (editor_image)
                                SDL_DestroyTexture(editor_image);

                            editor_image = 0;

                            m_tree->reset_default_rect();
                            m_tree->update_display();
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
                        if (m_selected_entry->mode() == gui::EntryMode::HIGHLIGHT)
                            m_selected_entry->erase_highlighted_section();

                        m_selected_entry->insert_char('\n');
                        m_tree->append_unsaved_file(current_open_fp, m_window);
                        
                        break;
                    case SDL_SCANCODE_BACKSPACE:
                        if (!mouse_down)
                        {
                            m_selected_entry->remove_char();
                            m_tree->append_unsaved_file(current_open_fp, m_window);
                        }
                        break;
                    case SDL_SCANCODE_LSHIFT:
                    case SDL_SCANCODE_RSHIFT:
                        shift_down = true;
                        break;
                    case SDL_SCANCODE_HOME:
                        if (shift_down)
                            m_selected_entry->start_highlight();
                        else
                            m_selected_entry->stop_highlight();

                        m_selected_entry->move_cursor_characters(-m_selected_entry->cursor().char_pos(m_selected_entry->rect()).x, 0);
                        m_selected_entry->reset_bounds_x();
                        break;
                    case SDL_SCANCODE_END:
                        if (shift_down)
                            m_selected_entry->start_highlight();
                        else
                            m_selected_entry->stop_highlight();

                        if (m_selected_entry->jump_to_eol())
                        {
                            m_selected_entry->move_bounds_characters(m_selected_entry->cursor().char_pos(m_selected_entry->rect()).x - m_selected_entry->max_bounds().x, 0);
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

                    bool moved = movement.x != 0 || movement.y != 0;

                    if (moved)
                    {
                        if (m_selected_entry->mode() == gui::EntryMode::NORMAL && shift_down)
                            m_selected_entry->start_highlight();

                        m_selected_entry->move_cursor_characters(movement.x, movement.y);

                        m_selected_entry->conditional_move_bounds_characters(
                            movement.x * m_selected_entry->move_bounds_by(),
                            movement.y * m_selected_entry->move_bounds_by()
                        );

                        m_selected_entry->conditional_jump_to_eol();

                        if (!shift_down && m_selected_entry->mode() == gui::EntryMode::HIGHLIGHT)
                            m_selected_entry->stop_highlight();
                    }
                }

                if (m_selected_basic_entry)
                {
                    switch (evt.key.keysym.scancode)
                    {
                    case SDL_SCANCODE_BACKSPACE:
                        m_selected_basic_entry->remove_char();
                        break;
                    }
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
                case SDL_SCANCODE_LSHIFT:
                case SDL_SCANCODE_RSHIFT:
                    shift_down = false;
                    break;
                }
            } break;
            case SDL_MOUSEWHEEL:
                if (gui::common::within_rect(m_tree->rect(), mx, my))
                {
                    m_tree->scroll(-evt.wheel.y, wy);
                }
                else if (gui::common::within_rect(m_text_entries[0].rect(), mx, my))
                {
                    m_text_entries[0].move_bounds_characters(0, -evt.wheel.y);
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
            btn->check_hover(mx, my);
            btn->render(m_rend);
        }

        m_tree->reload_outdated_folders(m_rend, false);
        m_tree->render(m_rend);

        if (gui::common::within_rect(m_tree->rect(), mx, my))
            m_tree->highlight_element(m_rend, mx, my);

        for (auto& e : m_text_entries)
        {
            if (e.hidden())
                continue;

            bool render_mouse = false;
            if (m_selected_entry == &e)
                render_mouse = true;

            e.render(m_rend, render_mouse);
        }


        if (!m_scrollbar.hidden())
        {
            SDL_Point min_bound = m_text_entries[0].min_bounds();
            SDL_Point max_bound = m_text_entries[0].max_bounds();

            if (!m_scrollbar.down())
            {
                m_scrollbar.set_bounds(min_bound.y, max_bound.y, m_text_entries[0].text()->contents().size() + (max_bound.y - min_bound.y));
            }
            else
            {
                m_scrollbar.move_with_cursor(my);

                m_text_entries[0].move_bounds_characters(0, m_scrollbar.min_position() - min_bound.y);
            }

            m_scrollbar.render(m_rend);
        }

        for (auto& e : m_basic_text_entries)
        {
            e.render(m_rend);
        }

        if (menu)
            menu->render(m_rend, mx, my);

        if (prev_wx != wx || prev_wy != wy)
        {
            m_text_entries[0].resize_to(wx - scrollbar_width, wy);
            m_tree->resize_to(wy);
            
            SDL_Rect entry_rect = m_text_entries[0].rect();
            m_scrollbar.move((entry_rect.x + entry_rect.w) - m_scrollbar.rect().x, 0);
            m_scrollbar.resize(wy);

            prev_wx = wx;
            prev_wy = wy;
        }

        if (editor_image)
        {
            int img_w, img_h;
            SDL_QueryTexture(editor_image, nullptr, nullptr, &img_w, &img_h);

            int img_x = (m_text_entries[0].rect().w / 2) - img_w / 2 + m_text_entries[0].rect().x;
            int img_y = (m_text_entries[0].rect().h / 2) - img_h / 2 + m_text_entries[0].rect().y;

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

    m_font_textbox.cleanup();
    m_font_tree.cleanup();

    m_text_entries.clear();
    m_basic_text_entries.clear();

    delete m_tree;
    
    for (auto& path : m_tree->unsaved())
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
