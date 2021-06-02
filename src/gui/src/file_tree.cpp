#include "file_tree.h"
#include "common.h"
#include <filesystem>
#include <iostream>
#include <SDL_image.h>

#if defined(_WIN32)
#  define PATH_SLASH '\\'
#else
#  define PATH_SLASH '/'
#endif /* if defined(_WIN32) */

namespace fs = std::filesystem;


gui::File::File(const std::string& base_path, const Text& name, SDL_Renderer* rend)
    : m_base_path(base_path), m_name(name), m_rect{ 0, 0, 0, 0 }
{
    m_tex = common::render_text(rend, m_name.font(), m_name.str().c_str(), m_name.color());
}


void gui::File::render(SDL_Renderer* rend, int offset, int top_y)
{
    if (m_rect.y >= top_y)
    {
        SDL_Rect tmp = {
            m_rect.x + offset,
            m_rect.y,
            m_name.char_dim().x * (int)m_name.str().size(),
            m_name.char_dim().y
        };

        SDL_RenderCopy(rend, m_tex, nullptr, &tmp);
    }
}


std::string gui::File::path()
{
    return m_base_path + PATH_SLASH + m_name.str();
}


gui::Folder::Folder(const std::string& base_path, const Text& name, SDL_Renderer* rend)
    : m_base_path(base_path), m_name(name), m_rect{ 0, 0, 0, 0 }
{
    Text t = name;
    std::string sname = m_name.str();

    for (auto& entry : fs::directory_iterator(m_base_path + PATH_SLASH + sname))
    {
        t.set_contents({ entry.path().filename().string() });
        
        if (entry.is_directory())
        {
            m_folders.emplace_back(Folder(m_base_path + (sname.empty() ? "" : PATH_SLASH + sname), t, rend));
        }
        else
        {
            m_files.emplace_back(File(m_base_path + (sname.empty() ? "" : PATH_SLASH + sname), t, rend));
        }
    }

    m_tex = common::render_text(rend, m_name.font(), m_name.str().c_str(), m_name.color());
}


void gui::Folder::render(SDL_Renderer* rend, int offset, SDL_Texture* closed_tex, SDL_Texture* opened_tex, int top_y)
{
    if (m_rect.y >= top_y)
    {
        SDL_Rect text_rect = {
            m_rect.x + offset,
            m_rect.y,
            m_name.char_dim().x * (int)m_name.str().size(),
            m_name.char_dim().y
        };

        SDL_RenderCopy(rend, m_tex, nullptr, &text_rect);

        SDL_Rect folder_rect = {
            text_rect.x - m_name.char_dim().x * 2,
            text_rect.y,
            m_name.char_dim().y,
            m_name.char_dim().y
        };

        if (m_collapsed)
        {
            SDL_RenderCopy(rend, closed_tex, nullptr, &folder_rect);
            return;
        }
        else
        {
            SDL_RenderCopy(rend, opened_tex, nullptr, &folder_rect);
        }
    }

    for (auto& folder : m_folders)
    {
        folder.render(rend, offset + 10, closed_tex, opened_tex, top_y);
    }

    for (auto& file : m_files)
    {
        file.render(rend, offset + 10, top_y);
    }
}


void gui::Folder::collapse()
{
    if (!m_collapsed)
    {
        m_collapsed = true;

        for (auto& folder : m_folders)
        {
            folder.collapse();
            folder.reset_rect();
        }

        for (auto& file : m_files)
        {
            file.reset_rect();
        }
    }
    else
    {
        m_collapsed = false;
    }
}


void gui::Folder::update_rects(SDL_Rect& rect)
{
    m_rect = rect;
    rect.y += m_name.char_dim().y;

    if (m_collapsed)
        return;

    for (auto& folder : m_folders)
    {
        folder.update_rects(rect);
    }

    for (auto& file : m_files)
    {
        file.update_rect(rect);
    }
}


gui::Tree::Tree(const Folder& folder, SDL_Rect starting_rect, SDL_Renderer* rend)
    : m_folder(folder), m_default_rect(starting_rect)
{
    m_closed_folder_texture = IMG_LoadTexture(rend, "res/folder_closed.png");
    m_opened_folder_texture = IMG_LoadTexture(rend, "res/folder_open.png");

    if (!m_closed_folder_texture || !m_opened_folder_texture)
    {
        std::cout << "failed to load tree textures\n";
    }

    m_top_y = m_default_rect.y;
}


void gui::Tree::render(SDL_Renderer* rend)
{
    SDL_Rect rect = m_default_rect;
    int offset = 20;

    for (auto& folder : m_folder.folders())
    {
        folder.render(rend, offset, m_closed_folder_texture, m_opened_folder_texture, m_top_y);
    }

    for (auto& file : m_folder.files())
    {
        file.render(rend, offset, m_top_y);
    }
}


gui::File* gui::Tree::check_file_click(Folder& folder, int mx, int my)
{
    for (auto& file : folder.files())
    {
        if (common::within_rect(file.rect(), mx, my))
            return &file;
    }

    for (auto& f : folder.folders())
    {
        File* file = check_file_click(f, mx, my);

        if (file)
            return file;
    }

    return nullptr;
}


gui::Folder* gui::Tree::check_folder_click(Folder& folder, int mx, int my)
{
    if (common::within_rect(folder.rect(), mx, my))
    {
        return &folder;
    }

    for (auto& f : folder.folders())
    {
        if (common::within_rect(f.rect(), mx, my))
            return &f;

        Folder* fold = check_folder_click(f, mx, my);

        if (fold)
            return fold;
    }

    return nullptr;
}


void gui::Tree::collapse_folder(Folder& folder)
{
    folder.collapse();
}


void gui::Tree::update_display()
{
    SDL_Rect rect = m_default_rect;
    int offset = 20;

    for (auto& folder : m_folder.folders())
    {
        folder.update_rects(rect);
    }

    for (auto& file : m_folder.files())
    {
        file.update_rect(rect);
    }
}


void gui::Tree::scroll(int y)
{
    m_default_rect.y -= m_folder.name().char_dim().y * y;
    update_display();
}