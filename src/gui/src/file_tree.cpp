#include "file_tree.h"
#include "common.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;


gui::File::File(const std::string& base_path, const Text& name, SDL_Renderer* rend)
    : m_base_path(base_path), m_name(name)
{
    m_tex = common::render_text(rend, m_name.font(), m_name.str().c_str(), m_name.color());
}


void gui::File::render(SDL_Renderer* rend, SDL_Rect rect, int offset)
{
    SDL_Rect tmp = {
        rect.x + offset,
        rect.y,
        m_name.char_dim().x * (int)m_name.str().size(),
        m_name.char_dim().y
    };

    SDL_RenderCopy(rend, m_tex, nullptr, &tmp);
}


gui::Folder::Folder(const std::string& base_path, const Text& name, SDL_Renderer* rend)
    : m_base_path(base_path), m_name(name)
{
    Text t = name;
    std::string sname = m_name.str();

    for (auto& entry : fs::directory_iterator(m_base_path + "\\" + sname))
    {
        t.set_contents({ entry.path().filename().string() });
        
        if (entry.is_directory())
        {
            m_folders.emplace_back(Folder(m_base_path + (sname.empty() ? "" : "\\" + sname), t, rend));
        }
        else
        {
            m_files.emplace_back(File(m_base_path + (sname.empty() ? "" : "\\" + sname), t, rend));
        }
    }

    m_tex = common::render_text(rend, m_name.font(), m_name.str().c_str(), m_name.color());
}


void gui::Folder::render(SDL_Renderer* rend, SDL_Rect& rect, int offset)
{
    SDL_Rect tmp = {
        rect.x + offset,
        rect.y,
        m_name.char_dim().x * (int)m_name.str().size(),
        m_name.char_dim().y
    };

    SDL_RenderCopy(rend, m_tex, nullptr, &tmp);

    for (auto& folder : m_folders)
    {
        rect.y += folder.name().char_dim().y;
        folder.render(rend, rect, offset + 10);
    }

    for (auto& file : m_files)
    {
        rect.y += file.name().char_dim().y;
        file.render(rend, rect, offset + 10);
    }
}


gui::Tree::Tree(const Folder& folder, SDL_Rect starting_rect, SDL_Color bg_color)
    : m_folder(folder), m_default_rect(starting_rect), m_bg_color(bg_color) {}


void gui::Tree::render(SDL_Renderer* rend)
{
    SDL_Rect rect = m_default_rect;
    int offset = 0;

    for (auto& folder : m_folder.folders())
    {
        folder.render(rend, rect, offset);
        rect.y += folder.name().char_dim().y;
    }

    for (auto& file : m_folder.files())
    {
        file.render(rend, rect, offset);
        rect.y += file.name().char_dim().y;
    }
}