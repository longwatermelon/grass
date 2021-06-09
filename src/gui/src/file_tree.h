#pragma once
#include "gui_string.h"
#include "common.h"
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>

namespace chrono = std::chrono;


namespace gui
{
    class File
    {
    public:
        File() = default;
        File(const std::string& base_path, const String& name, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, int offset, int top_y, std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>>& file_textures, std::vector<std::string>& unsaved_files);

        void update_rect(SDL_Rect& r) { m_rect = r; r.y += m_name.char_dim().y; }

        void delete_self();

        std::string path();
        String name() const { return m_name; }
        SDL_Rect rect() const { return m_rect; }

        void reset_rect() { m_rect = { -1, -1, -1, -1 }; }

    private:
        SDL_Rect m_rect;

        std::string m_base_path;
        String m_name;

        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_tex;

        bool m_saved{ true };
    };

    class Folder
    {
    public:
        Folder(const std::string& base_path, const String& name, SDL_Renderer* rend, bool load_directory);

        void render(SDL_Renderer* rend, int offset, SDL_Texture* closed_tex, SDL_Texture* opened_tex, int top_y, std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>>& file_textures, std::vector<std::string>& unsaved_files);

        /* If already collapsed, folder will expand. */
        void collapse(SDL_Renderer* rend);

        /* Update where all the folders and files will be rendered. */
        void update_rects(SDL_Rect& rect);

        /* Load all the contents directly beneath this folder. This is used to save memory. */
        void load(SDL_Renderer* rend);
        /* Unload all the contents from the folder, normally called when the folder is closed. */
        void unload();

        void change_directory(const std::string& fp, SDL_Renderer* rend);

        Folder* find_lowest_folder();
        File* find_lowest_file();
        Folder* find_last_folder_with_files();

        SDL_Rect find_lowest_rect();

        void create_new_file(const std::string& name);

        void reload_if_outdated(SDL_Renderer* rend);


        std::vector<File>& files() { return m_files; }
        std::vector<Folder>& folders() { return m_folders; }
        String name() const { return m_name; }
        SDL_Rect rect() const { return m_rect; }
        std::string path() { return m_base_path + '/' + m_name.str(); }

        bool loaded() { return m_loaded; }

        void reset_rect() { m_rect = { -1, -1, -1, -1 }; }

    private:
        SDL_Rect m_rect;

        std::string m_base_path;
        String m_name;

        std::vector<File> m_files;
        std::vector<Folder> m_folders;

        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_tex;

        bool m_loaded{ false };
    };

    class Tree
    {
    public:
        Tree(SDL_Rect rect, Folder& folder, SDL_Rect starting_rect, SDL_Renderer* rend);

        void render(SDL_Renderer* rend);

        File* check_file_click(Folder& folder, int mx, int my);
        Folder* check_folder_click(Folder& folder, int mx, int my);

        // collapses folder if not already collapsed, otherwise expands the folder
        void collapse_folder(Folder& folder, SDL_Renderer* rend);

        void update_display();

        void scroll(int y, int window_h);

        void append_unsaved_file(const std::string& fp, SDL_Window* window);
        void erase_unsaved_file(const std::string& fp, SDL_Window* window);
        bool is_unsaved(const std::string& fp);

        // highlights a file / folder based off where mouse position is
        void highlight_element(SDL_Renderer* rend, int mx, int my);
        void resize_to(int h) { m_rect.h = h; }

        void reset_default_rect();

        void reload_outdated_folders(SDL_Renderer* rend, bool force_reload);

        void set_selected_highlight_rect(SDL_Rect rect) { m_selected_highlight_rect = rect; }
        Folder& folder() { return m_folder; }
        std::vector<std::string> unsaved() { return m_unsaved_files; }
        SDL_Rect rect() { return m_rect; }

    private:
        SDL_Rect m_rect;

        Folder m_folder;
        SDL_Rect m_default_rect;

        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_opened_folder_texture;
        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_closed_folder_texture;

        std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>> m_file_textures;

        std::vector<std::string> m_unsaved_files;

        SDL_Rect m_selected_highlight_rect{ 0, 0, 0, 0 };

        chrono::system_clock::time_point m_last_filesystem_check{ chrono::system_clock::now() };
    };
}