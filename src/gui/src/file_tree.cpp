#include "file_tree.h"
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <fstream>
#include <set>
#include <SDL_image.h>

#define unique(ptr) std::unique_ptr<SDL_Texture, common::TextureDeleter>(ptr)

namespace fs = std::filesystem;


gui::File::File(const std::string& base_path, const String& name, SDL_Renderer* rend)
    : m_base_path(base_path), m_name(name), m_rect{ 0, 0, 0, 0 }
{
    m_tex = unique(common::render_text(rend, m_name.font(), m_name.str().c_str(), m_name.color()));
}


void gui::File::render(SDL_Renderer* rend, int offset, int top_y, std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>>& file_textures, std::vector<std::string>& unsaved_files, SDL_Rect tree_rect)
{
    m_rect.x = offset;
    m_rect.w = (tree_rect.x + tree_rect.w) - m_rect.x;

    if (m_rect.y >= top_y)
    {
        SDL_Rect text_rect = {
            offset,
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


void gui::File::delete_self()
{
    fs::remove(m_base_path + '/' + m_name.str());
}


std::string gui::File::path()
{
    return m_base_path + '/' + m_name.str();
}


gui::Folder::Folder(const std::string& base_path, const String& name, SDL_Renderer* rend, bool load_directory)
    : m_base_path(base_path), m_name(name), m_rect{ 0, 0, 0, 0 }, m_loaded(load_directory)
{
    if (load_directory)
        load(rend);

    m_tex = unique(common::render_text(rend, m_name.font(), m_name.str().c_str(), m_name.color()));
}


void gui::Folder::render(SDL_Renderer* rend, int offset, SDL_Texture* closed_tex, SDL_Texture* opened_tex, int top_y, std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>>& file_textures, std::vector<std::string>& unsaved_files, SDL_Rect tree_rect)
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

        if (!m_loaded)
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
        folder.render(rend, offset + 10, closed_tex, opened_tex, top_y, file_textures, unsaved_files, tree_rect);
    }

    for (auto& file : m_files)
    {
        file.render(rend, offset + 10, top_y, file_textures, unsaved_files, tree_rect);
    }
}


void gui::Folder::collapse(SDL_Renderer* rend)
{
    if (m_loaded)
    {
        unload();
    }
    else
    {
        load(rend);
    }
}


void gui::Folder::update_rects(SDL_Rect& rect)
{
    m_rect = rect;
    rect.y += m_name.char_dim().y;

    if (!m_loaded)
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
    m_loaded = true;
    load_folders(rend);
    load_files(rend);

    for (auto& f : m_folders)
    {
        if (f.loaded())
        {
            f.load(rend);
        }
    }
}


void gui::Folder::load_folders(SDL_Renderer* rend)
{
    std::vector<std::string> loaded_paths = find_all_loaded_folders();

    for (int i = 0; i < m_folders.size(); ++i)
    {
        // if folder is not loaded, erase it
        if (std::find(loaded_paths.begin(), loaded_paths.end(), fs::absolute(m_folders[i].path()).string()) == loaded_paths.end())
        {
            m_folders.erase(m_folders.begin() + i);
            --i;
        }
    }

    String s = m_name;
    std::string name = m_name.str();

    for (auto& entry : fs::directory_iterator(m_base_path + '/' + name, fs::directory_options::skip_permission_denied))
    {
        if (!entry.is_directory() || std::find(loaded_paths.begin(), loaded_paths.end(), fs::absolute(entry.path()).string()) != loaded_paths.end())
            continue;

        s.set_contents({ entry.path().filename().string() });
        
        m_folders.emplace_back(Folder(m_base_path + (name.empty() ? "" : '/' + name), s, rend, false));
    }

    std::set<std::string> sorted;

    for (auto& f : m_folders)
    {
        sorted.insert(f.name().str());
    }

    std::vector<Folder> tmp;
    
    for (auto& str : sorted)
    {
        for (auto& f : m_folders)
        {
            if (f.name().str() == str)
            {
                tmp.emplace_back(std::move(f));
                break;
            }
        }
    }

    m_folders = std::move(tmp);
}


void gui::Folder::load_files(SDL_Renderer* rend)
{
    m_files.clear();

    String s = m_name;
    std::string name = m_name.str();

    for (auto& entry : fs::directory_iterator(m_base_path + '/' + name, fs::directory_options::skip_permission_denied))
    {
        if (entry.is_directory())
            continue;

        std::string file_name = entry.path().filename().string();
        std::string extension = fs::path(file_name).extension().string();

        if (fs::path(file_name).has_extension() && extension[extension.size() - 1] == '~')
            continue;

        s.set_contents({ file_name });
        m_files.emplace_back(File(m_base_path + (name.empty() ? "" : '/' + name), s, rend));
    }
}


void gui::Folder::unload()
{
    m_loaded = false;
    m_files.clear();
    m_folders.clear();
}


void gui::Folder::change_directory(const std::string& fp, SDL_Renderer* rend)
{
    fs::path p(fp);

    m_base_path = p.parent_path().string();
    m_name.set_contents({ p.filename().string() });

    unload();
    load(rend);
}


gui::Folder* gui::Folder::find_lowest_folder()
{
    if (m_folders.empty())
        return 0;

    Folder* f = m_folders[m_folders.size() - 1].find_lowest_folder();

    if (!f)
        return &m_folders[m_folders.size() - 1];
    else
    {
        Folder* tmp = f->find_lowest_folder();

        if (tmp)
            return tmp;
        else
            return f;
    }
}


gui::File* gui::Folder::find_lowest_file()
{
    if (m_files.empty())
        return 0;

    return &m_files[m_files.size() - 1];
}


gui::Folder* gui::Folder::find_last_folder_with_files()
{
    if (!m_files.empty())
        return this;

    for (int i = m_folders.size() - 1; i >= 0; --i)
    {
        Folder* tmp = m_folders[i].find_last_folder_with_files();

        if (tmp)
            return tmp;
    }

    return 0;
}


SDL_Rect gui::Folder::find_lowest_rect()
{
    Folder* f = find_lowest_folder();

    if (!f)
    {
        if (m_files.empty())
            return m_rect;
        else
        {
            File* file = find_lowest_file();

            return file->rect();
        }
    }
    
    File* file = find_lowest_file();

    if (file)
        return file->rect();
    else
    {
        Folder* first_folder = find_last_folder_with_files();
        Folder* last_folder = find_lowest_folder();

        if (first_folder == last_folder)
            return last_folder->rect();

        if (first_folder)
            file = first_folder->find_lowest_file();

        if (file)
            return file->rect();
        else
            return f->rect();
    }
}


void gui::Folder::create_new_file(const std::string& name)
{
    std::ofstream ofs(m_base_path + '/' + m_name.str() + '/' + name);
    ofs.close();
}


void gui::Folder::create_new_folder(const std::string& name)
{
    fs::create_directory(m_base_path + '/' + m_name.str() + '/' + name);
}


void gui::Folder::reload_if_outdated(SDL_Renderer* rend, bool unconditional)
{
    size_t count = 0;

    for (auto& entry : fs::directory_iterator(path(), fs::directory_options::skip_permission_denied))
        ++count;

    if (count != m_folders.size() + m_files.size() || unconditional)
    {
        load(rend);
    }

    for (auto& folder : m_folders)
    {
        if (folder.loaded())
            folder.reload_if_outdated(rend);
    }
}


std::vector<std::string> gui::Folder::find_all_loaded_folders()
{
    std::vector<std::string> loaded;

    for (auto& f : m_folders)
    {
        std::vector<std::string> tmp = f.find_all_loaded_folders();
        loaded.insert(loaded.end(), tmp.begin(), tmp.end());
    }

    for (auto& f : m_folders)
    {
        if (f.loaded())
        {
            loaded.emplace_back(fs::absolute(f.path()).string());
        }
    }

    return loaded;
}


gui::Folder* gui::Folder::folder_from_path(const std::string& full_path)
{
    if (path() == full_path)
        return this;

    for (auto& folder : m_folders)
    {
        if (folder.path() == full_path)
            return &folder;
        else
        {
            gui::Folder* f = folder.folder_from_path(full_path);
            if (f)
                return f;
        }
    }

    return 0;
}


gui::File* gui::Folder::file_from_path(const std::string& file_path)
{
    for (auto& file : m_files)
    {
        if (file.path() == file_path)
            return &file;
    }

    for (auto& folder : m_folders)
    {
        gui::File* file = folder.file_from_path(file_path);

        if (file)
        {
            return file;
        }
    }

    return 0;
}


void gui::Folder::remove_self()
{
    std::error_code ec;
    fs::remove_all(m_base_path + '/' + m_name.str(), ec);

    if (ec)
        std::cout << "error: " << ec.message() << "\n";
}


gui::Tree::Tree(SDL_Rect rect, Folder& folder, SDL_Rect starting_rect, SDL_Renderer* rend, const std::string& exe_dir)
    : m_folder(std::move(folder)), m_default_rect(starting_rect), m_rect(rect)
{
    m_closed_folder_texture = unique(IMG_LoadTexture(rend, (exe_dir + "res/folder_closed.png").c_str()));
    m_opened_folder_texture = unique(IMG_LoadTexture(rend, (exe_dir + "res/folder_open.png").c_str()));

    m_file_textures["na"] = unique(IMG_LoadTexture(rend, (exe_dir + "res/file_na.png").c_str()));
    m_file_textures["na_unsaved"] = unique(IMG_LoadTexture(rend, (exe_dir + "res/file_na_unsaved.png").c_str()));
}


void gui::Tree::render(SDL_Renderer* rend)
{
    SDL_Rect rect = m_default_rect;
    int offset = 20;

    for (auto& folder : m_folder.folders())
    {
        folder.render(rend, offset, m_closed_folder_texture.get(), m_opened_folder_texture.get(), m_rect.y, m_file_textures, m_unsaved_files, m_rect);
    }

    for (auto& file : m_folder.files())
    {
        file.render(rend, offset, m_rect.y, m_file_textures, m_unsaved_files, m_rect);
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
    //File& last_file = m_folder.files()[m_folder.files().size() - 1];
    SDL_Rect bottom_rect = m_folder.find_lowest_rect();
    int char_height = m_folder.name().char_dim().y;

    if (m_default_rect.y - y * char_height <= m_rect.y)
    {
        if (y > 0) // scrolling downwards, everything moves up
        {
            if (bottom_rect.y + bottom_rect.h - y * char_height + char_height >= window_h)
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

    //File& bottom_file = m_folder.files()[m_folder.files().size() - 1];
    SDL_Rect lowest_rect = m_folder.find_lowest_rect();

    if (rect.y > lowest_rect.y)
        return;

    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 50);
    SDL_RenderFillRect(rend, &rect);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
}


void gui::Tree::reset_default_rect()
{
    m_default_rect.y = m_rect.y;
}


void gui::Tree::reload_outdated_folders(SDL_Renderer* rend, bool force_reload, bool unconditional_reload)
{
    if (force_reload || chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - m_last_filesystem_check).count() > 250)
    {
        m_last_filesystem_check = chrono::system_clock::now();

        m_folder.reload_if_outdated(rend, unconditional_reload);
        update_display();
    }
}


gui::Folder* gui::Tree::folder_from_path(const std::string& full_path)
{
    return m_folder.folder_from_path(full_path);
}


gui::File* gui::Tree::file_from_path(const std::string& full_path)
{
    return m_folder.file_from_path(full_path);
}