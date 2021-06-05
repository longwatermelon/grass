#pragma once
#include "text.h"
#include "common.h"
#include <string>
#include <vector>
#include <memory>
#include <map>


namespace gui
{
    class File
    {
    public:
        File() = default;
        File(const std::string& base_path, const Text& name, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, int offset, int top_y, std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>>& file_textures, std::vector<std::string>& unsaved_files);

        void update_rect(SDL_Rect& r) { m_rect = r; r.y += m_name.char_dim().y; }

        std::string path();
        Text name() const { return m_name; }
        SDL_Rect rect() const { return m_rect; }

        void reset_rect() { m_rect = { -1, -1, -1, -1 }; }

    private:
        SDL_Rect m_rect;

        std::string m_base_path;
        Text m_name;

        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_tex;

        bool m_saved{ true };
    };

    class Folder
    {
    public:
        Folder(const std::string& base_path, const Text& name, SDL_Renderer* rend, bool load_directory);

        void render(SDL_Renderer* rend, int offset, SDL_Texture* closed_tex, SDL_Texture* opened_tex, int top_y, std::map<std::string, std::unique_ptr<SDL_Texture, common::TextureDeleter>>& file_textures, std::vector<std::string>& unsaved_files);

        /* If already collapsed, folder will expand. */
        void collapse(SDL_Renderer* rend);

        /* Update where all the folders and files will be rendered. */
        void update_rects(SDL_Rect& rect);

        /* Load all the contents directly beneath this folder. This is used to save memory. */
        void load(SDL_Renderer* rend);
        /* Unload all the contents from the folder, normally called when the folder is closed. */
        void unload();

        std::vector<File>& files() { return m_files; }
        std::vector<Folder>& folders() { return m_folders; }
        Text name() const { return m_name; }
        SDL_Rect rect() const { return m_rect; }

        void reset_rect() { m_rect = { -1, -1, -1, -1 }; }

    private:
        SDL_Rect m_rect;

        std::string m_base_path;
        Text m_name;

        std::vector<File> m_files;
        std::vector<Folder> m_folders;

        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_tex;

        bool m_collapsed{ false };
        bool m_loaded{ false };
    };

    class Tree
    {
    public:
        Tree(SDL_Rect rect, Folder& folder, SDL_Rect starting_rect, SDL_Renderer* rend);

        void render(SDL_Renderer* rend);

        File* check_file_click(Folder& folder, int mx, int my);
        Folder* check_folder_click(Folder& folder, int mx, int my);

        /* Collapses folder, if folder is already collapsed it will be expanded. */
        void collapse_folder(Folder& folder, SDL_Renderer* rend);

        void update_display();

        void scroll(int y, int window_h);

        void append_unsaved_file(const std::string& fp, SDL_Window* window);
        void erase_unsaved_file(const std::string& fp, SDL_Window* window);
        bool is_unsaved(const std::string& fp);

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
    };
}