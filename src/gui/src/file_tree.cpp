#include "file_tree.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <SDL_image.h>

#if defined(_WIN32)
#  define PATH_SLASH '\\'
#else
#  define PATH_SLASH '/'
#endif /* if defined(_WIN32) */

#define unique(ptr) std::unique_ptr<SDL_Texture, common::TextureDeleter>(ptr)

namespace fs = std::filesystem;


gui::File::File(const std::string& base_path, const Text& name, SDL_Renderer* rend)
    : m_base_path(base_path), m_name(name), m_rect{ 0, 0, 0, 0 }
{
    m_tex = unique(common::render_text(rend, m_name.font(), m_name.str().c_str(), m_name.color()));
}


void gui::File::render(SDL_Renderer* rend, int offset, int top_y, std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>>& file_textures, std::vector<std::string>& unsaved_files)
{
    if (m_rect.y >= top_y)
    {
        SDL_Rect text_rect = {
            m_rect.x + offset,
            m_rect.y
        };

        SDL_QueryTexture(m_tex.get(), nullptr, nullptr, &text_rect.w, &text_rect.h);
        SDL_RenderCopy(rend, m_tex.get(), nullptr, &text_rect);

        SDL_Rect icon_rect = {
            text_rect.x - m_name.char_dim().x * 2 - 2,
            text_rect.y,
            m_name.char_dim().y,
            m_name.char_dim().y
        };

        // not unsaved
        if (std::find(unsaved_files.begin(), unsaved_files.end(), path()) == unsaved_files.end())
            SDL_RenderCopy(rend, file_textures["na"].get(), nullptr, &icon_rect);
        else
            SDL_RenderCopy(rend, file_textures["na_unsaved"].get(), nullptr, &icon_rect);
    }
}


std::string gui::File::path()
{
    return m_base_path + PATH_SLASH + m_name.str();
}


gui::Folder::Folder(const std::string& base_path, const Text& name, SDL_Renderer* rend, bool load_directory)
    : m_base_path(base_path), m_name(name), m_rect{ 0, 0, 0, 0 }, m_loaded(load_directory)
{
    if (load_directory)
        load(rend);

    m_tex = unique(common::render_text(rend, m_name.font(), m_name.str().c_str(), m_name.color()));
}


void gui::Folder::render(SDL_Renderer* rend, int offset, SDL_Texture* closed_tex, SDL_Texture* opened_tex, int top_y, std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>>& file_textures, std::vector<std::string>& unsaved_files)
{
    if (m_rect.y >= top_y)
    {
        SDL_Rect text_rect = {
            m_rect.x + offset,
            m_rect.y
        };

        SDL_QueryTexture(m_tex.get(), nullptr, nullptr, &text_rect.w, &text_rect.h);
        SDL_RenderCopy(rend, m_tex.get(), nullptr, &text_rect);

        SDL_Rect folder_rect = {
            text_rect.x - m_name.char_dim().x * 2 - 2,
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
        folder.render(rend, offset + 10, closed_tex, opened_tex, top_y, file_textures, unsaved_files);
    }

    for (auto& file : m_files)
    {
        file.render(rend, offset + 10, top_y, file_textures, unsaved_files);
    }
}


void gui::Folder::collapse(SDL_Renderer* rend)
{
    if (!m_collapsed)
    {
        m_collapsed = true;

        unload();
    }
    else
    {
        m_collapsed = false;
        load(rend);

        for (auto& f : m_folders)
        {
            f.collapse(rend);
        }
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


void gui::Folder::load(SDL_Renderer* rend)
{
    Text t = m_name;
    std::string sname = m_name.str();

    std::error_code ec;
    for (auto& entry : fs::directory_iterator(m_base_path + PATH_SLASH + sname, fs::directory_options::skip_permission_denied, ec))
    {
        if (ec)
            continue;

        std::string file_path = entry.path().filename().string();
        std::string ext = fs::path(file_path).extension().string();

        if (fs::path(file_path).has_extension() && ext[ext.size() - 1] == '~')
            continue;

        t.set_contents({ file_path });
        

        if (entry.is_directory())
        {
            m_folders.emplace_back(Folder(m_base_path + (sname.empty() ? "" : PATH_SLASH + sname), t, rend, false));
        }
        else
        {
            m_files.emplace_back(File(m_base_path + (sname.empty() ? "" : PATH_SLASH + sname), t, rend));
        }
    }
}


void gui::Folder::unload()
{
    m_files.clear();
    m_folders.clear();
}


gui::Tree::Tree(SDL_Rect rect, Folder& folder, SDL_Rect starting_rect, SDL_Renderer* rend)
    : m_folder(std::move(folder)), m_default_rect(starting_rect), m_rect(rect)
{
    m_closed_folder_texture = unique(IMG_LoadTexture(rend, "res/folder_closed.png"));
    m_opened_folder_texture = unique(IMG_LoadTexture(rend, "res/folder_open.png"));

    m_file_textures["na"] = unique(IMG_LoadTexture(rend, "res/file_na.png"));
    m_file_textures["na_unsaved"] = unique(IMG_LoadTexture(rend, "res/file_na_unsaved.png"));
}


void gui::Tree::render(SDL_Renderer* rend)
{
    SDL_Rect rect = m_default_rect;
    int offset = 20;

    for (auto& folder : m_folder.folders())
    {
        folder.render(rend, offset, m_closed_folder_texture.get(), m_opened_folder_texture.get(), m_rect.y, m_file_textures, m_unsaved_files);
    }

    for (auto& file : m_folder.files())
    {
        file.render(rend, offset, m_rect.y, m_file_textures, m_unsaved_files);
    }

    if (m_selected_highlight_rect.y >= m_rect.y)
    {
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 100);
        SDL_RenderFillRect(rend, &m_selected_highlight_rect);
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
    }
}


gui::File* gui::Tree::check_file_click(Folder& folder, int mx, int my)
{
    if (my < m_rect.y)
        return nullptr;

    for (auto& file : folder.files())
    {
        if (common::within_rect(file.rect(), mx, my))
        {
            return &file;
        }
    }

    for (auto& f : folder.folders())
    {
        File* file = check_file_click(f, mx, my);

        if (file)
        {
            return file;
        }
    }

    return nullptr;
}


gui::Folder* gui::Tree::check_folder_click(Folder& folder, int mx, int my)
{
    if (my < m_rect.y)
        return nullptr;

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


void gui::Tree::collapse_folder(Folder& folder, SDL_Renderer* rend)
{
    folder.collapse(rend);
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


void gui::Tree::scroll(int y, int window_h)
{
    File& last_file = m_folder.files()[m_folder.files().size() - 1];
    Folder& first_folder = m_folder.folders()[0];
    int char_height = m_folder.name().char_dim().y;

    if (first_folder.rect().y - y * char_height <= m_rect.y)
    {
        if (y > 0) // scrolling downwards, everything moves up
        {
            if (last_file.rect().y + last_file.rect().h - y * char_height + char_height >= window_h)
            {
                m_default_rect.y -= char_height * y;
                m_selected_highlight_rect.y -= char_height * y;
            }
        }
        else // scrolling upwards, everything moves down
        {
            m_default_rect.y -= char_height * y;
            m_selected_highlight_rect.y -= char_height * y;
        }
    }

    update_display();
}


void gui::Tree::append_unsaved_file(const std::string& fp, SDL_Window* window)
{
    if (fp.empty())
        return;

    if (std::find(m_unsaved_files.begin(), m_unsaved_files.end(), fp) == m_unsaved_files.end())
    {
        m_unsaved_files.emplace_back(fp);
        SDL_SetWindowTitle(window, (std::string(SDL_GetWindowTitle(window)) + std::string(" - UNSAVED")).c_str());
    }
}


void gui::Tree::erase_unsaved_file(const std::string& fp, SDL_Window* window)
{
    auto pos = std::find(m_unsaved_files.begin(), m_unsaved_files.end(), fp);

    if (pos != m_unsaved_files.end())
    {
        m_unsaved_files.erase(pos);

        std::string title = SDL_GetWindowTitle(window);
        title = title.substr(0, title.size() - std::string(" - UNSAVED").size());
        
        SDL_SetWindowTitle(window, title.c_str());

        if (fs::exists(fp + "~"))
        {
            fs::remove(fp + "~");
        }
    }
}


bool gui::Tree::is_unsaved(const std::string& fp)
{
    return std::find(m_unsaved_files.begin(), m_unsaved_files.end(), fp) != m_unsaved_files.end();
}


void gui::Tree::highlight_element(SDL_Renderer* rend, int mx, int my)
{
    SDL_Rect rect = {
        m_rect.x,
        (int)((my - m_rect.y) / m_folder.name().char_dim().y) * m_folder.name().char_dim().y + m_rect.y,
        m_rect.w,
        m_folder.name().char_dim().y
    };

    File& bottom_file = m_folder.files()[m_folder.files().size() - 1];
    
    if (rect.y > bottom_file.rect().y)
        return;

    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 50);
    SDL_RenderFillRect(rend, &rect);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
}