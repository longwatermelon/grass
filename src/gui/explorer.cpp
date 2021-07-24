#include "explorer.h"
#include "button.h"
#include "text.h"
#include "common.h"
#include <iostream>
#include <chrono>
#include <thread>

namespace chrono = std::chrono;


gui::Explorer::Explorer(const std::string& dir, ExplorerMode mode, SDL_Point pos, const std::string& exe_dir, common::Font& font)
    : m_current_dir(fs::path(dir).lexically_normal().string()), m_mode(mode), m_font(font)
{
    m_window = SDL_CreateWindow((std::string("Select ") + (mode == ExplorerMode::DIR ? "directory" : "file")).c_str(), pos.x, pos.y, 800, 400, SDL_WINDOW_HIDDEN);
    m_rend = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    SDL_RenderClear(m_rend);
    SDL_RenderPresent(m_rend);

    SDL_SetWindowGrab(m_window, SDL_TRUE);
}


std::string gui::Explorer::get_path()
{
    SDL_ShowWindow(m_window);

    bool running = true;
    SDL_Event evt;

    bool return_path = false;

    SDL_Point window_size;
    SDL_GetWindowSize(m_window, &window_size.x, &window_size.y);

    int entry_start_x = 50;
    int entry_width = 400;

    int bottom_menu_height = 70;

    // y position of first entry
    int top_y = 0;

    chrono::system_clock::time_point first_click_time = chrono::system_clock::now();
    chrono::system_clock::time_point second_click_time = chrono::system_clock::now();
    bool ready_for_first_click = true;
    std::string first_clicked_item;

    Text path_text(m_rend, m_font, { entry_start_x, window_size.y - 50 }, "Path: " + fs::absolute(fs::path(m_current_dir)).string(), { 255, 255, 255 });


    SDL_Point button_pos = { window_size.x - 100, window_size.y - 25 };

    std::vector<Button*> buttons;
    buttons.emplace_back(new Button(m_rend, String(m_font, button_pos, "Select", { 255, 255, 255 }), { button_pos.x, button_pos.y, 95, 20 }, { 100, 100, 100 }, [&]() {
        running = false;
        return_path = true;
    }));

    button_pos.x -= 100;
    buttons.emplace_back(new Button(m_rend, String(m_font, button_pos, "Cancel", { 255, 255, 255 }), { button_pos.x, button_pos.y, 95, 20 }, { 100, 100, 100 }, [&]() {
        running = false;
    }));

    button_pos.x -= 200;
    buttons.emplace_back(new Button(m_rend, String(m_font, button_pos, "Move up a dir", { 255, 255, 255 }), { button_pos.x, button_pos.y, 195, 20 }, { 100, 100, 100 }, [&]() {
        m_current_dir = fs::absolute(fs::path(m_current_dir)).parent_path().string();
        m_selected_item.clear();
        m_selected_item_highlight = { 0, 0, 0, 0 };
        path_text.set_text("Path: " + m_current_dir);
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
            {
                bool clicked = false;

                for (auto& btn : buttons)
                {
                    if (btn->check_clicked(mx, my))
                        clicked = true;
                }

                if (!clicked && my < window_size.y - bottom_menu_height)
                {
                    m_selected_item = elem_at_mouse_pos(my, m_font.char_dim().y, top_y);

                    path_text.set_text("Path: " + fs::absolute(fs::path(m_current_dir + (m_selected_item.empty() ? "" : "/" + m_selected_item))).string());

                    // clicked a valid item
                    if (!m_selected_item.empty())
                    {
                        m_selected_item_highlight = {
                            entry_start_x,
                            (int)(my / m_font.char_dim().y) * m_font.char_dim().y,
                            entry_width,
                            m_font.char_dim().y
                        };

                        if (ready_for_first_click)
                        {
                            first_click_time = chrono::system_clock::now();
                            ready_for_first_click = false;
                            first_clicked_item = m_selected_item;
                        }
                        else if (chrono::duration_cast<chrono::milliseconds>(second_click_time - first_click_time).count() < 500)
                        {
                            if (m_selected_item != first_clicked_item)
                            {
                                ready_for_first_click = false;
                            }
                            else // double click was successful
                            {
                                char last_char = m_current_dir[m_current_dir.size() - 1];

                                m_current_dir += (m_selected_item.empty() ? "" : (last_char == '\\' || last_char == '/' ? "" : "/") + m_selected_item);
                                m_selected_item_highlight = { 0, 0, 0, 0 };
                                m_selected_item.clear();
                                ready_for_first_click = true;
                                first_clicked_item.clear();
                                top_y = 0;

                                for (auto& t : m_current_textures)
                                {
                                    if (t)
                                    {
                                        SDL_DestroyTexture(t);
                                        t = 0;
                                    }
                                }
                            }
                        }
                    }
                }

                break;
            } break;
            case SDL_MOUSEBUTTONUP:
            {
                for (auto& btn : buttons)
                {
                    btn->set_down(false);
                }
            } break;
            case SDL_MOUSEWHEEL:
            {
                int diff = evt.wheel.y * m_font.char_dim().y;
                int last_visible_y = m_current_names.size() * m_font.char_dim().y + top_y;

                if ((top_y + diff <= 0 && diff > 0) || (last_visible_y >= window_size.y - bottom_menu_height && diff < 0))
                {
                    top_y += diff;
                    m_selected_item_highlight.y += diff;
                }

            } break;
            }
        }

        SDL_RenderClear(m_rend);

        second_click_time = chrono::system_clock::now();

        if (!ready_for_first_click && chrono::duration_cast<chrono::milliseconds>(second_click_time - first_click_time).count() > 500)
            ready_for_first_click = true;

        update_current_directory();
        render_current_directory(m_font, entry_start_x, top_y);

        if (my < window_size.y - bottom_menu_height)
            highlight_elem_at_mouse(mx, my, m_font.char_dim().y, entry_start_x, entry_width, top_y);

        SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(m_rend, 255, 255, 255, 100);
        SDL_RenderFillRect(m_rend, &m_selected_item_highlight);
        SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_NONE);

        SDL_SetRenderDrawColor(m_rend, 50, 50, 50, 255);

        {
            SDL_Rect rect = { 0, window_size.y - bottom_menu_height, window_size.x, bottom_menu_height };

            SDL_RenderFillRect(m_rend, &rect);
        }

        for (auto& btn : buttons)
        {
            if (btn)
            {
                btn->check_hover(mx, my);
                btn->render(m_rend);
            }
        }

        path_text.render();

        SDL_SetRenderDrawColor(m_rend, 30, 30, 30, 255);
        SDL_RenderPresent(m_rend);
        
        if (!running)
        {
            cleanup(buttons);
            SDL_HideWindow(m_window);

            if (return_path)
                return m_current_dir + (m_selected_item.empty() ? "" : '/' + m_selected_item);
            else
                return "";
        }
    }

    return "";
}


void gui::Explorer::cleanup(std::vector<Button*>& buttons)
{
    for (auto& tex : m_current_textures)
    {
        if (tex)
            SDL_DestroyTexture(tex);
    }

    for (auto& btn : buttons)
    {
        delete btn;
        btn = 0;
    }

    buttons.clear();
    m_current_textures.clear();
    m_current_names.clear();

    SDL_SetWindowGrab(m_window, SDL_FALSE);
}


void gui::Explorer::cleanup_window()
{
    SDL_DestroyRenderer(m_rend);
    SDL_DestroyWindow(m_window);
}


void gui::Explorer::update_current_directory()
{
    size_t old_name_count = m_current_names.size();
    m_current_names.clear();

    for (auto& entry : fs::directory_iterator(m_current_dir, fs::directory_options::skip_permission_denied))
    {
        if (entry.is_directory())
        {
            if (m_mode == ExplorerMode::DIR)
            {
                m_current_names.emplace_back(entry.path().filename().string());
            }
        }
        else
        {
            if (m_mode == ExplorerMode::FILE)
            {
                m_current_names.emplace_back(entry.path().filename().string());
            }
        }
    }

    if (old_name_count != m_current_names.size())
    {
        for (auto& tex : m_current_textures)
        {
            if (tex)
                SDL_DestroyTexture(tex);

            tex = 0;
        }

        m_current_textures.clear();
    }
}


void gui::Explorer::render_current_directory(common::Font& font, int entry_start_x, int top_y)
{
    SDL_Rect current_text_rect = { entry_start_x, top_y };

    for (int i = 0; i < m_current_names.size(); ++i)
    {
        if (i >= m_current_textures.size())
        {
            m_current_textures.emplace_back(nullptr);
        }

        if (!m_current_textures[i])
        {
            m_current_textures[i] = common::render_text(m_rend, font.font(), m_current_names[i].c_str());
        }

        if (m_current_textures[i])
        {
            SDL_QueryTexture(m_current_textures[i], 0, 0, &current_text_rect.w, &current_text_rect.h);
            SDL_RenderCopy(m_rend, m_current_textures[i], 0, &current_text_rect);
            current_text_rect.y += font.char_dim().y;
        }
    }
}


std::string gui::Explorer::elem_at_mouse_pos(int my, int font_dim_y, int top_y)
{
    int index = (int)((my - top_y) / font_dim_y);

    if (index < m_current_names.size())
        return m_current_names[index];
    else
        return "";
}


void gui::Explorer::highlight_elem_at_mouse(int mx, int my, int font_dim_y, int entry_start_x, int entry_width, int top_y)
{
    if ((int)((my - top_y) / font_dim_y) >= m_current_names.size() || my - top_y < 0 || mx < entry_start_x || mx > entry_start_x + entry_width)
        return;

    int y = (my / font_dim_y) * font_dim_y;
    SDL_Rect rect = { entry_start_x, y, entry_width, font_dim_y };

    SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(m_rend, 255, 255, 255, 50);
    SDL_RenderFillRect(m_rend, &rect);
    SDL_SetRenderDrawBlendMode(m_rend, SDL_BLENDMODE_NONE);
}
