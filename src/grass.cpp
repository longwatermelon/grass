#include "grass.h"
#include "gui/common.h"
#include "gui/button.h"
#include "gui/file_tree.h"
#include "gui/text_entry.h"
#include "gui/explorer.h"
#include "gui/scrollbar.h"
#include "gui/menu.h"
#include "gui/basic_text_entry.h"
#include "gui/tab.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <thread>
#include <memory>
#include <sstream>
#include <map>
#include <SDL_image.h>

namespace fs = std::filesystem;


Grass::Grass(const std::string& exe_dir)
    : m_exe_dir(exe_dir + '/')
{
    SDL_Init(SDL_INIT_VIDEO);

    m_window = SDL_CreateWindow("Grass", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1000, 800, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    TTF_Init();
    IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

    m_font_textbox.load_font(m_exe_dir + "res/fonts/CascadiaCode.ttf", 16);
    m_font_tree.load_font(m_exe_dir + "res/fonts/CascadiaCode.ttf", 14);

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
    /* Core ui elements that should not be touched */
    m_text_entries.emplace_back(gui::TextEntry(m_main_text_dimensions, { 30, 30, 30 }, gui::Cursor({ m_main_text_dimensions.x, m_main_text_dimensions.y }, { 255, 255, 255 }, m_font_textbox.char_dim_ref()), gui::String(m_font_textbox, { m_main_text_dimensions.x, m_main_text_dimensions.y }, "", { 255, 255, 255 })));

    gui::Folder folder(fs::absolute(".").string(), gui::String(m_font_tree, { 0, 60 }, "", { 255, 255, 255 }), m_rend, true);

    m_tree = new gui::Tree(
        { 0, m_main_text_dimensions.y, m_main_text_dimensions.x, 800 - m_main_text_dimensions.y },
        folder,
        // when changing font size make sure to also change the 20 below to the y value of the char dimensions specified above
        { 0, m_main_text_dimensions.y, 200, m_font_tree.char_dim().y },
        m_rend,
        m_exe_dir
    );

    m_tree->update_display();

    m_scrollbar = gui::Scrollbar({
        m_main_text_dimensions.x + m_main_text_dimensions.w,
        m_main_text_dimensions.y,
        m_scrollbar_width,
        m_main_text_dimensions.h
        }, 0, 0, 0, { 40, 40, 40 }, { 100, 100, 100 });

    bool running = true;
    SDL_Event evt;

    /* Extra misc variables that should be added to */

    std::string m_current_open_fp;
    SDL_Texture* editor_image = nullptr;

    int prev_wx, prev_wy;
    SDL_GetWindowSize(m_window, &prev_wx, &prev_wy);

    gui::Menu* menu = 0;

    std::string renamed_fp;

    gui::common::Font explorer_font(m_exe_dir + "res/fonts/CascadiaCode.ttf", 14);

    SDL_Point pos;
    SDL_GetWindowPosition(m_window, &pos.x, &pos.y);

    m_explorer = new gui::Explorer(m_tree->folder().path(), gui::ExplorerMode::DIR, pos, m_exe_dir, explorer_font);

    // put the buttons here so they have access to all the previous variables

    m_buttons.emplace_back(new gui::Button(m_rend, gui::String(m_font_tree, { 0, 0 }, "Help", { 255, 255, 255 }), { 0, 0, 60, 20 }, { 70, 70, 70 }, [&]() {
        if (editor_image)
            SDL_DestroyTexture(editor_image);

        editor_image = 0;

        load_file(m_exe_dir + "res/misc/help.txt");
        }));

    m_buttons.emplace_back(new gui::Button(m_rend, gui::String(m_font_tree, { m_text_entries[0].rect().x + m_text_entries[0].rect().w, m_tab_y }, ">", { 255, 255, 255 }), { m_text_entries[0].rect().x + m_text_entries[0].rect().w, m_tab_y, 20, 20 }, { 70, 70, 70 }, [&]() {
        gui::Tab* first_visible = get_first_visible_tab();
        int index = 0;

        for (int i = 0; i < m_file_tabs.size(); ++i)
        {
            if (first_visible == m_file_tabs[i].get())
            {
                index = i;
                break;
            }
        }
            
        if (first_visible && index != m_file_tabs.size() - 1)
        {
            for (auto& t : m_file_tabs)
            {
                t->move(-(first_visible->text_pixel_length()) - m_tab_gap);
            }
        }
    }));

    m_buttons.emplace_back(new gui::Button(m_rend, gui::String(m_font_tree, { m_text_entries[0].rect().x, m_tab_y }, "<", { 255, 255, 255 }), { m_text_entries[0].rect().x, m_tab_y, 20, 20 }, { 70, 70, 70 }, [&]() {
        if (m_file_tabs.size() > 1)
        {
            gui::Tab* first_invisible = get_first_invisible_tab();

            if (first_invisible)
            {
                for (auto& t : m_file_tabs)
                {
                    t->move(first_invisible->text_pixel_length() + m_tab_gap);
                }
            }
        }
    }));
    
    std::vector<std::string> plugins;
    for (auto& entry : fs::directory_iterator(m_exe_dir + "res/plugins", fs::directory_options::skip_permission_denied))
    {
        plugins.emplace_back(fs::absolute(entry.path()).string()); 
    }

    PluginManager manager(plugins);
    manager.run_plugins();
    configure_from_plugins(manager);

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
            if (evt.type == SDL_WINDOWEVENT && evt.window.event == SDL_WINDOWEVENT_CLOSE)
            {
                running = false;
                break;
            }

            switch (evt.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                handle_mouse_down(evt.button.button, mouse_down, mx, my, menu, editor_image, renamed_fp);
                break;

            case SDL_MOUSEBUTTONUP:
                handle_mouse_up(mouse_down);
                break;

            case SDL_TEXTINPUT:
                handle_textinput(evt.text.text[0]);
                break;

            case SDL_KEYDOWN:
                handle_keydown(evt, ctrl_down, shift_down, mouse_down, editor_image, { wx, wy }, renamed_fp);
                break;

            case SDL_KEYUP:
                handle_keyup(evt, shift_down, ctrl_down);
                break;

            case SDL_MOUSEWHEEL:
                handle_mousewheel(evt, mx, my, wy, ctrl_down);
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

        m_tree->reload_outdated_folders(m_rend, false);
        m_tree->render(m_rend);

        // menu should be on the topmost layer so nothing else should be being highlighted when the menu is up
        if (gui::common::within_rect(m_tree->rect(), mx, my) && !menu)
            m_tree->highlight_element(m_rend, mx, my);

        for (auto& e : m_text_entries)
        {
            if (e.hidden())
                continue;

            e.render(m_rend);
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

        for (auto& t : m_file_tabs)
        {
            if (t->rect().x < m_tab_start)
                continue;
            
            if (mx < m_text_entries[0].rect().x + m_text_entries[0].rect().w)
                t->hover_highlight(mx, my);
            else
                t->set_hover(false);

            if (m_selected_tab)
                m_selected_tab->set_clicked(true);

            t->render(m_rend);
        }

        for (auto& btn : m_buttons)
        {
            btn->check_hover(mx, my);
            btn->render(m_rend);
        }

        if (menu)
            menu->render(m_rend, mx, my);

        if (prev_wx != wx || prev_wy != wy)
        {
            m_text_entries[0].resize_to(wx - m_scrollbar_width, wy);
            m_tree->resize_to(wy);
            
            // move the button on the right to fit the screen width
            m_buttons[1]->move_to(m_text_entries[0].rect().x + m_text_entries[0].rect().w, m_buttons[2]->rect().y);

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

            // make sure the image still fits in the text box
            if (img_x < m_main_text_dimensions.x)
            {
                int diff = m_main_text_dimensions.x - img_x;
                dstrect.x += diff;
                dstrect.y += diff;
                dstrect.w -= diff;
                dstrect.h -= diff;
            }

            if (dstrect.x + dstrect.w >= m_text_entries[0].rect().x + m_text_entries[0].rect().w)
            {
                int diff = (m_text_entries[0].rect().x + m_text_entries[0].rect().w) - (dstrect.x + dstrect.w);
                dstrect.w += diff;
                dstrect.h += diff;
            }

            SDL_RenderCopy(m_rend, editor_image, nullptr, &dstrect);
        }

        SDL_SetRenderDrawColor(m_rend, BG_COLOR.r, BG_COLOR.g, BG_COLOR.b, 255);
        SDL_RenderPresent(m_rend);
    }

    m_font_textbox.cleanup();
    m_font_tree.cleanup();

    m_text_entries.clear();
    m_basic_text_entries.clear();

    m_buttons.clear();

    for (auto& path : m_tree->unsaved())
    {
        if (fs::exists(path + "~"))
            fs::remove(path + "~");
    }

    delete m_tree;

    m_explorer->cleanup_window();
    delete m_explorer;

    if (editor_image)
        SDL_DestroyTexture(editor_image);

    if (menu)
        delete menu;

    m_file_tabs.clear();
}


void Grass::load_file(const std::string& fp)
{
    std::ifstream ifs(fp);

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(ifs, line)) lines.emplace_back(line);

    ifs.close();

    m_text_entries[0].text()->set_contents(lines);
    m_text_entries[0].update_cache();
    reset_entry_to_default(m_text_entries[0]);

    SDL_SetWindowTitle(m_window, ("Grass | Editing " + fs::path(fp).filename().string() + (m_tree->is_unsaved(fs::absolute(fp).string()) ? " - UNSAVED" : "")).c_str());    
    
    std::string path = fp;
    if (path[path.size() - 1] == '~')
        path.pop_back();
    
    if (!fs::exists(path))
    {
        SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Error", ("'" + fs::path(path).lexically_normal().string() + "' doesn't exist").c_str(), m_window);
        remove_tab(path);
        return;
    }
    else
    {
        append_tab(path);
        select_tab(path);
    }
    
    std::string extension = fs::path(path).extension().string();

    if (!extension.empty())
        extension.erase(extension.begin());

    m_text_entries[0].reset_all_keywords();

    for (auto& pair : m_control_flow_keywords)
    {
        if (pair.first == extension)
        {
            m_text_entries[0].set_control_flow_keywords(pair.second);
            break;
        } 
    }

    for (auto& pair : m_types_keywords)
    {
        if (pair.first == extension)
        {
            m_text_entries[0].set_types_keywords(pair.second);
            break;
        }
    }

    for (auto& pair : m_constants_keywords)
    {
        if (pair.first == extension)
        {
            m_text_entries[0].set_constants_keywords(pair.second);
            break;
        }
    }

    for (auto& pair : m_misc_keywords)
    {
        if (pair.first == extension)
        {
            m_text_entries[0].set_misc_keywords(pair.second);
            break;
        }
    }

    for (auto& pair : m_comment_indicators)
    {
        if (pair.first == extension)
        {
            m_text_entries[0].set_comment_indicators(pair.second);
            break;
        }
    }
}

void Grass::close_current_file(std::string& m_current_open_fp)
{
    reset_entry_to_default(m_text_entries[0]);
    m_text_entries[0].text()->set_contents({ "" });
    SDL_SetWindowTitle(m_window, "Grass");
    
    if (m_selected_tab && tab_from_path(m_current_open_fp) == m_selected_tab)
    {
        m_selected_tab->set_clicked(false);
        m_selected_tab = 0;
    }

    m_current_open_fp.clear();
}


void Grass::reset_entry_to_default(gui::TextEntry& entry)
{
    entry.reset_bounds_x();
    entry.reset_bounds_y();
    entry.set_cursor_pos_characters(0, 0);
    entry.update_cache();
    entry.show();
}


void Grass::handle_mouse_down(Uint8 button, bool& mouse_down, int mx, int my, gui::Menu*& menu, SDL_Texture*& editor_image, std::string& renamed_file)
{
    if (m_mode == Mode::FILE_RENAME && !m_selected_basic_entry->check_clicked(mx, my))
    {
        m_selected_basic_entry = 0;
        m_basic_text_entries.pop_back();
        m_mode = Mode::NORMAL;
    }

    if (button == SDL_BUTTON_LEFT && !mouse_down)
    {
        mouse_down = true;
        bool clicked_something = false;

        if (menu)
        {
            menu->check_clicked(mx, my);

            delete menu;
            menu = 0;
            clicked_something = true;
        }

        if (!clicked_something)
        {
            for (auto& btn : m_buttons)
            {
                if (btn->check_clicked(mx, my))
                {
                    clicked_something = true;
                    break;
                }
            }
        }

        bool clicked_entry = false;

        if (!clicked_something)
        {
            for (auto& e : m_text_entries)
            {
                if (e.check_clicked(mx, my))
                {
                    clicked_entry = true;
                    clicked_something = true;

                    m_selected_entry = &e;
                    m_selected_entry->mouse_down(mx, my);
                    m_selected_entry->set_cursor_shown(true);

                    break;
                }
            }
        }

        if (!clicked_entry)
        {
            m_selected_entry = 0;

            for (auto& e : m_text_entries)
            {
                e.stop_highlight();
                e.set_cursor_shown(false);
            }
        }

        bool clicked_basic_entry = false;

        if (!clicked_something)
        {
            for (auto& e : m_basic_text_entries)
            {
                if (e.check_clicked(mx, my))
                {
                    clicked_basic_entry = true;
                    clicked_something = true;

                    m_selected_basic_entry = &e;
                    m_selected_basic_entry->set_cursor_visible(true);
                }
            }
        }

        if (!clicked_basic_entry && m_mode == Mode::NORMAL)
        {
            for (auto& e : m_basic_text_entries)
            {
                e.set_cursor_visible(false);
            }

            m_selected_basic_entry = 0;
        }
        
        if (!clicked_something)
        {
            for (int i = 0; i < m_file_tabs.size(); ++i)
            {
                auto& tab = m_file_tabs[i];

                if (tab->check_clicked(mx, my))
                {
                    clicked_something = true;
                    std::string path = tab->path();
                    load_file(path);

                    if (fs::exists(path))
                    {
                        m_selected_tab = tab.get();
                        m_current_open_fp = tab->path();
                    }
                    else
                    {
                        --i;
                    }
                }
                else
                {
                    tab->set_clicked(false);
                }
            }
        }

        if (m_selected_tab)
        {
            m_selected_tab->set_clicked(true);
        }
        

        gui::File* file = m_tree->check_file_click(m_tree->folder(), mx, my);

        if (file && !clicked_something)
        {
            if (!fs::exists(file->path()))
            {
                SDL_ShowSimpleMessageBox(SDL_MessageBoxFlags::SDL_MESSAGEBOX_ERROR, "Error", std::string("'" + file->path() + "' doesn't exist").c_str(), m_window);
                return;
            }

            m_tree->set_selected_highlight_rect({
                m_tree->rect().x,
                file->rect().y,
                m_tree->rect().w,
                file->name().char_dim().y
            });

            if (m_tree->is_unsaved(m_current_open_fp))
            {
                std::ofstream ofs(m_current_open_fp + "~", std::ofstream::out | std::ofstream::trunc);
                ofs << m_text_entries[0].text()->str();
                ofs.close();
            }

            m_text_entries[0].stop_highlight();

            m_current_open_fp = file->path();

            if (m_selected_tab)
               m_selected_tab->set_clicked(false); 

            m_selected_tab = tab_from_path(m_current_open_fp);
            std::string ext = fs::path(m_current_open_fp).extension().string();

            // try and cover all common image file extensions even if they are not supported
            if (ext == ".png" || ext == ".jpg" || ext == ".bmp" || ext == ".jpeg" || ext == ".webp")
            {
                editor_image = IMG_LoadTexture(m_rend, m_current_open_fp.c_str());

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

                if (fs::exists(m_current_open_fp + "~"))
                    load_file(m_current_open_fp + "~");
                else
                    load_file(m_current_open_fp);

                m_selected_tab = tab_from_path(m_current_open_fp);
            }

            m_tree->update_display();
        }

        gui::Folder* folder = m_tree->check_folder_click(m_tree->folder(), mx, my);

        if (folder && !clicked_something)
        {
            m_tree->collapse_folder(*folder, m_rend);
            m_tree->update_display();

            m_tree->set_selected_highlight_rect({
                m_tree->rect().x,
                folder->rect().y,
                m_tree->rect().w,
                folder->name().char_dim().y
                });
        }

        if (!folder && !file)
            m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
        else
            clicked_something = true;

        if (!clicked_something)
            m_scrollbar.check_clicked(mx, my);
    }

    if (button == SDL_BUTTON_RIGHT)
    {
        gui::Folder* f = m_tree->check_folder_click(m_tree->folder(), mx, my);

        if (f)
        {
            SDL_Rect rect = f->rect();
            renamed_file = f->path();

            menu = new gui::Menu({ mx, my }, 150, {
                {"New file", [&, path = f->path()]() {
                    gui::Folder* folder = m_tree->folder_from_path(path);
                    folder->create_new_file("New file");
                    folder->load(m_rend);
                    m_tree->update_display();
                    m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                }},
                {"New folder",[&, path = f->path()]() {
                    gui::Folder* folder = m_tree->folder_from_path(path);
                    folder->create_new_folder("New folder");
                    folder->load(m_rend);
                    m_tree->update_display();
                    m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                }},
                {"Delete folder",[&, path = f->path()]() {
                    gui::Folder* folder = m_tree->folder_from_path(path);

                    if (folder->loaded())
                        folder->collapse(m_rend);

                    folder->remove_self();
                    m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                    m_tree->reload_outdated_folders(m_rend, true);

                    if (!fs::exists(m_current_open_fp))
                    {
                        close_current_file(m_current_open_fp);
                    }
                }},
                {"Rename", [&, r = rect]() {
                    m_mode = Mode::FILE_RENAME;

                    int rect_w = m_text_entries[0].rect().x - r.x;
                    int line_num_width = (int)std::to_string(m_text_entries[0].text()->contents().size()).size() * m_font_textbox.char_dim().x + m_font_textbox.char_dim().x;

                    m_basic_text_entries.emplace_back(gui::BasicTextEntry(
                        {
                            r.x,
                            r.y,
                            rect_w - line_num_width,
                            m_font_tree.char_dim().y 
                        },
                        gui::Cursor({ r.x, r.y }, { 255, 255, 255 }, m_font_tree.char_dim_ref()),
                        std::make_unique<gui::Text>(gui::Text(m_rend, m_font_tree, { r.x, r.y }, "", { 255, 255, 255 })),
                        { 40, 40, 40 }
                    ));

                    m_selected_basic_entry = &m_basic_text_entries[m_basic_text_entries.size() - 1];
                    m_selected_basic_entry->set_cursor_visible(true);

                    if (!fs::exists(m_current_open_fp))
                    {
                        close_current_file(m_current_open_fp);
                    }
                }}
                }, m_font_tree, { 40, 40, 40 }, m_rend);
        }

        gui::File* file = m_tree->check_file_click(m_tree->folder(), mx, my);

        if (file)
        {
            SDL_Rect rect = file->rect();
            renamed_file = file->path();

            menu = new gui::Menu({ mx, my }, 150, {
                {"Delete file", [&, path = file->path()]() {
                    gui::File* file = m_tree->file_from_path(path);

                    if (m_current_open_fp == path)
                    {
                        reset_entry_to_default(m_text_entries[0]);
                        m_text_entries[0].text()->set_contents({ "" });

                        m_tree->erase_unsaved_file(file->path(), m_window);

                        if (fs::exists(file->path() + '~'))
                            fs::remove(file->path() + '~');
                    }
                    
                    if (tab_exists(file->path()))
                        remove_tab(file->path()); 

                    file->delete_self();
                    m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                    m_tree->reload_outdated_folders(m_rend, true);

                    if (!fs::exists(m_current_open_fp))
                    {
                        close_current_file(m_current_open_fp);
                    }
                }},
                {"Rename",[&, r = rect]() {
                    m_mode = Mode::FILE_RENAME;

                    int rect_w = m_text_entries[0].rect().x - r.x;
                    int line_num_width = (int)std::to_string(m_text_entries[0].text()->contents().size()).size() * m_font_textbox.char_dim().x + m_font_textbox.char_dim().x;

                    m_basic_text_entries.emplace_back(gui::BasicTextEntry(
                        {
                        r.x,
                        r.y,
                        rect_w - line_num_width,
                        m_font_tree.char_dim().y
                        },
                        gui::Cursor({ r.x, r.y }, { 255, 255, 255 }, m_font_tree.char_dim_ref()),
                        std::make_unique<gui::Text>(gui::Text(m_rend, m_font_tree, { r.x, r.y }, "", { 255, 255, 255 })),
                        { 40, 40, 40 })
                    );

                    m_selected_basic_entry = &m_basic_text_entries[m_basic_text_entries.size() - 1];
                    m_selected_basic_entry->set_cursor_visible(true);
                }}
                }, m_font_tree, { 40, 40, 40 }, m_rend);
        }

        if (!file && !f)
        {
            if (gui::common::within_rect(m_tree->rect(), mx, my))
            {
                if (menu)
                {
                    delete menu;
                    menu = 0;
                }

                menu = new gui::Menu({ mx, my }, 150, {
                    {"New file", [&]() {
                        m_tree->folder().create_new_file("New file");
                        m_tree->folder().load(m_rend);
                        m_tree->update_display();
                        m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                    }},
                    {"New folder", [&]() {
                        m_tree->folder().create_new_folder("New folder");
                        m_tree->folder().load(m_rend);
                        m_tree->update_display();
                        m_tree->set_selected_highlight_rect({ 0, 0, 0, 0 });
                    }}
                    }, m_font_tree, { 40, 40, 40 }, m_rend);
            }
        }

        gui::Tab* clicked_tab = get_clicked_tab(mx, my);

        if (clicked_tab)
        {
            menu = new gui::Menu({ mx, my }, 150, {
                {"Remove from list", [&, path = clicked_tab->path()] () {
                    remove_tab(path); 
                }}
            }, m_font_tree, { 40, 40, 40 }, m_rend);            
        }
    }
}


void Grass::handle_mouse_up(bool& mouse_down)
{
    mouse_down = false;

    if (m_selected_entry)
    {
        m_selected_entry->mouse_up();
    }

    for (auto& btn : m_buttons)
    {
        btn->set_down(false);
    }

    m_scrollbar.mouse_up();
}


void Grass::handle_textinput(char c)
{
    if (m_selected_entry)
    {
        if (m_selected_entry->mode() == gui::EntryMode::HIGHLIGHT)
            m_selected_entry->erase_highlighted_section();

        m_selected_entry->insert_char(c);
        m_tree->append_unsaved_file(m_current_open_fp, m_window);
    }

    if (m_selected_basic_entry)
    {
        m_selected_basic_entry->add_char(c);
    }
}


void Grass::handle_keydown(SDL_Event& evt, bool& ctrl_down, bool& shift_down, bool& mouse_down, SDL_Texture* editor_image, SDL_Point window_dim, std::string& renamed_file)
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
            if (ctrl_down && m_selected_entry == &m_text_entries[0] && !m_current_open_fp.empty())
            {
                std::ofstream ofs(m_current_open_fp, std::ofstream::out | std::ofstream::trunc);

                for (auto& line : m_text_entries[0].text()->contents())
                {
                    ofs << line << "\n";
                }

                ofs.close();

                m_tree->erase_unsaved_file(m_current_open_fp, m_window);
            }
        }

        break;
    case SDLK_d:
        if (m_selected_entry)
        {
            if (ctrl_down && m_selected_entry == &m_text_entries[0] && !m_current_open_fp.empty())
            {
                m_tree->erase_unsaved_file(m_current_open_fp, m_window);
                load_file(m_current_open_fp);
            }
        }

        break;
    case SDLK_o:
        if (ctrl_down)
        {
            SDL_Rect rect = { 0, 0, window_dim.x, window_dim.y };
            SDL_SetRenderDrawColor(m_rend, 0, 0, 0, 255);
            SDL_RenderFillRect(m_rend, &rect);

            std::string waiting_text = "Waiting for open folder dialog to finish";
            SDL_Texture* text = gui::common::render_text(m_rend, m_font_textbox.font(), waiting_text.c_str());
            rect.x = window_dim.x / 2 - waiting_text.size() * m_font_textbox.char_dim().x / 2;
            rect.y = window_dim.y / 2 - m_font_textbox.char_dim().y / 2;

            SDL_QueryTexture(text, nullptr, nullptr, &rect.w, &rect.h);
            SDL_RenderCopy(m_rend, text, nullptr, &rect);

            SDL_RenderPresent(m_rend);

            // new window will take all input and releasing ctrl wont be detected in the main window
            ctrl_down = false;

            std::string path = m_explorer->get_path();

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
        {
            if (m_selected_entry->mode() == gui::EntryMode::HIGHLIGHT)
                m_selected_entry->erase_highlighted_section();

            size_t tab_pos = m_selected_entry->get_tab_position();

            m_selected_entry->insert_char('\n');

            for (size_t i = 0; i < tab_pos; ++i)
                m_selected_entry->insert_char(' ');

            m_tree->append_unsaved_file(m_current_open_fp, m_window);
        } break;
        case SDL_SCANCODE_BACKSPACE:
            if (!mouse_down)
            {
                m_selected_entry->remove_char();
                m_tree->append_unsaved_file(m_current_open_fp, m_window);
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
        case SDL_SCANCODE_TAB:
            if (m_selected_entry->mode() == gui::EntryMode::HIGHLIGHT)
            {
                m_selected_entry->erase_highlighted_section();
                m_selected_entry->stop_highlight();
            }

            for (int i = 0; i < 4; ++i)
                m_selected_entry->insert_char(' ');

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
        case SDL_SCANCODE_RETURN:
            if (m_mode == Mode::FILE_RENAME)
            {
                if (tab_exists(renamed_file))
                {
                    remove_tab(renamed_file);
                }
                
                bool renamed_is_selected = fs::equivalent(m_current_open_fp, renamed_file);

                std::string new_name = m_selected_basic_entry->text();
                std::string full_new_path = fs::path(renamed_file).parent_path().string() + '/' + new_name;
                fs::rename(renamed_file, full_new_path);
                
                if (renamed_is_selected)
                {
                    m_current_open_fp = full_new_path;
                    load_file(m_current_open_fp); 
                }
                else
                {
                    append_tab(full_new_path); 
                }

                renamed_file.clear();
                m_basic_text_entries.pop_back();
                m_mode = Mode::NORMAL;
                m_selected_basic_entry = 0;

                m_tree->reload_outdated_folders(m_rend, true, true);
            }
            break;
        case SDL_SCANCODE_ESCAPE:
            if (m_mode == Mode::FILE_RENAME)
            {
                m_basic_text_entries.pop_back();
                m_selected_basic_entry = 0;
            }

            break;
        }
    }
}


void Grass::handle_keyup(SDL_Event& evt, bool& shift_down, bool& ctrl_down)
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
}


void Grass::handle_mousewheel(SDL_Event& evt, int mx, int my, int wy, bool ctrl_down)
{
    if (gui::common::within_rect(m_tree->rect(), mx, my))
    {
        m_tree->scroll(-evt.wheel.y, wy);
    }
    else if (gui::common::within_rect(m_text_entries[0].rect(), mx, my))
    {
        if (ctrl_down)
            m_text_entries[0].resize_text(m_font_textbox.pt_size() + evt.wheel.y * 2);
        else
            m_text_entries[0].move_bounds_characters(0, -evt.wheel.y);
    }
}


bool Grass::tab_exists(const std::string& fp)
{
    return tab_from_path(fp); 
}


gui::Tab* Grass::get_clicked_tab(int mx, int my)
{
    for (auto& tab : m_file_tabs)
    {
        if (tab->check_clicked(mx, my))
        {
            return tab.get();
        }
    }

    return 0;
}


gui::Tab* Grass::tab_from_path(const std::string& path)
{
    for (auto& t : m_file_tabs)
    {
        if (fs::path(t->path()).lexically_normal() == fs::path(path).lexically_normal())
        {
            return t.get();
        }
    }

    return 0;
}


gui::Tab* Grass::get_first_visible_tab()
{
    for (auto& t : m_file_tabs)
    {
        if (t->rect().x >= m_tab_start)
           return t.get();
    }

    return 0;
}


gui::Tab* Grass::get_first_invisible_tab()
{
    for (int i = 0; i < m_file_tabs.size(); ++i)
    {
        if (m_file_tabs[i]->rect().x >= m_tab_start)
        {
            if (i - 1 >= 0)
                return m_file_tabs[i - 1].get();
            else
                return 0;
        }
    }
    
    return m_file_tabs[m_file_tabs.size() - 1].get();
}


gui::Tab* Grass::get_last_visible_tab()
{
    for (int i = m_file_tabs.size() - 1; i > 0; --i)
    {
        if (m_file_tabs[i]->rect().x <= m_text_entries[0].rect().x + m_text_entries[0].rect().w)
            return m_file_tabs[i].get();
    }

    return m_file_tabs[m_file_tabs.size() - 1].get();
}


void Grass::remove_tab(const std::string& fp)
{
    bool found_target_removed = false;
    int distance = 0;
    
    for (int i = 0; i < m_file_tabs.size(); ++i)
    {
        if (!found_target_removed)
        {
            if (fs::path(m_file_tabs[i]->path()).lexically_normal() == fs::path(fp).lexically_normal())
            {
                if (m_file_tabs[i].get() == m_selected_tab)
                {
                    close_current_file(m_current_open_fp);
                }

                distance = m_file_tabs[i]->text_pixel_length();
                m_file_tabs.erase(m_file_tabs.begin() + i); 
                found_target_removed = true;
                --i;
            }
        }
        else
        {
            m_file_tabs[i]->move(-distance - m_tab_gap);
        }
    }

    if (!get_first_visible_tab())
    {
        gui::Tab* first_invisible = get_first_invisible_tab();
        if (first_invisible)
        {
            for (int i = 0; i < m_file_tabs.size(); ++i)
            {
                m_file_tabs[i]->move(first_invisible->text_pixel_length() + m_tab_gap);
            }
        }
    }
}


void Grass::append_tab(const std::string& full_path)
{
    if (!tab_exists(fs::absolute(full_path).string()))
    {
        gui::Tab* tab = 0;
        int offset = 0;
        int text_len = 0;
        if (m_file_tabs.size() > 0) 
        {
            tab = m_file_tabs[m_file_tabs.size() - 1].get();
            offset = tab->text()->rect().x - m_tab_start + m_tab_gap;
            text_len = tab->text_pixel_length();
        }

        m_file_tabs.emplace_back(
            new gui::Tab(
                std::make_unique<gui::Text>(gui::Text(m_rend, m_font_tree, { m_tab_start + offset  + text_len, m_tab_y }, fs::path(full_path).filename().string(), { 255, 255, 255 })),
                { 60, 60, 60 },
                fs::absolute(fs::path(full_path)).string(),
                20
            )
        );
    }
}


void Grass::select_tab(const std::string& full_path)
{
    m_selected_tab = tab_from_path(full_path);
    
    if (m_selected_tab)
    {
        while (m_selected_tab->rect().x > get_last_visible_tab()->rect().x)
        {
            gui::Tab* first = get_first_visible_tab();

            for (auto& t : m_file_tabs)
            {
                t->move(-(first->text_pixel_length() + m_tab_gap));
            } 
        }

        if (m_selected_tab->rect().x < get_first_visible_tab()->rect().x)
        {
            int diff = m_tab_start - m_selected_tab->rect().x;

            for (auto& t : m_file_tabs)
            {
                t->move(diff);
            }
        } 
    }
}


void Grass::configure_from_plugins(PluginManager& manager)
{
    plugin::Node* bg_color = manager.get_variable_from_name("bg_color");

    if (bg_color)
    {
        std::stringstream ss(bg_color->string_value);
        
        std::vector<Uint8> color(3);
        for (int i = 0; i < 3; ++i)
        {
            std::string line;
            std::getline(ss, line, ' ');
            color[i] = (Uint8)std::stoi(line); 
        }

        BG_COLOR = { color[0], color[1], color[2] };
    }

    std::vector<Plugin*> language_plugins = manager.get_plugins_containing_variable("language_pack");

    for (auto& plugin : language_plugins)
    {
        std::stringstream ss(plugin->variable_from_name("extensions")->variable_definition_value->string_value);
        std::vector<std::string> extensions;
        std::string buf;

        while (std::getline(ss, buf, ' ')) extensions.emplace_back(buf);
        
        std::stringstream control_flow(plugin->variable_from_name("control")->variable_definition_value->string_value);
        std::vector<std::string> control_flow_list;
        
        while (std::getline(control_flow, buf, ' ')) control_flow_list.emplace_back(buf);

        for (auto& ext : extensions)
        {
            m_control_flow_keywords[ext] = control_flow_list;
        }


        std::stringstream constants(plugin->variable_from_name("constants")->variable_definition_value->string_value);
        std::vector<std::string> constants_list;

        while (std::getline(constants, buf, ' ')) constants_list.emplace_back(buf);

        for (auto& ext : extensions)
        {
            m_constants_keywords[ext] = constants_list;
        }

        std::stringstream types(plugin->variable_from_name("types")->variable_definition_value->string_value);
        std::vector<std::string> types_list;

        while (std::getline(types, buf, ' ')) types_list.emplace_back(buf);

        for (auto& ext : extensions)
        {
            m_types_keywords[ext] = types_list;
        }

        std::stringstream misc(plugin->variable_from_name("misc")->variable_definition_value->string_value);
        std::vector<std::string> misc_list;

        while (std::getline(misc, buf, ' ')) misc_list.emplace_back(buf);

        for (auto& ext : extensions)
        {
            m_misc_keywords[ext] = misc_list;
        }
    }

    auto assign_keywords_to_extensions = [&](Plugin* plugin, std::map<std::string, std::vector<std::string>>& syntax_map, const std::string& variable_name, const std::vector<std::string>& extensions) {
        std::stringstream ss(plugin->variable_from_name(variable_name)->variable_definition_value->string_value);
        std::vector<std::string> list;
        std::string buf;

        while (std::getline(ss, buf, ' ')) list.emplace_back(buf);

        for (auto& ext : extensions)
        {
            syntax_map[ext] = list;
        }
    };

    for (auto& plugin : language_plugins)
    {
        std::stringstream ss(plugin->variable_from_name("extensions")->variable_definition_value->string_value);
        std::vector<std::string> extensions;
        std::string buf;

        while (std::getline(ss, buf, ' ')) extensions.emplace_back(buf);

        assign_keywords_to_extensions(plugin, m_control_flow_keywords, "control", extensions);
        assign_keywords_to_extensions(plugin, m_constants_keywords, "constants", extensions);
        assign_keywords_to_extensions(plugin, m_types_keywords, "types", extensions);
        assign_keywords_to_extensions(plugin, m_misc_keywords, "misc", extensions);
        assign_keywords_to_extensions(plugin, m_comment_indicators, "comment", extensions);
    }
}

