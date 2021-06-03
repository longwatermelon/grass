#pragma once
#include "text.h"
#include "common.h"
#include <string>
#include <vector>
#include <memory>


namespace gui
{
    class File
    {
    public:
        File() = default;
        File(const std::string& base_path, const Text& name, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, int offset, int top_y);

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
    };

    class Folder
    {
    public:
        Folder(const std::string& base_path, const Text& name, SDL_Renderer* rend, bool load_directory);

        void render(SDL_Renderer* rend, int offset, SDL_Texture* closed_tex, SDL_Texture* opened_tex, int top_y);

        /* If already collapsed, folder will expand. */
        void collapse(SDL_Renderer* rend);

        void update_rects(SDL_Rect& rect);

        void load(SDL_Renderer* rend);
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
        Tree(Folder& folder, SDL_Rect starting_rect, SDL_Renderer* rend);

        void render(SDL_Renderer* rend);

        File* check_file_click(Folder& folder, int mx, int my);
        Folder* check_folder_click(Folder& folder, int mx, int my);

        void collapse_folder(Folder& folder, SDL_Renderer* rend);

        void update_display();

        void scroll(int y, int window_h);

        Folder& folder() { return m_folder; }

    private:
        Folder m_folder;
        SDL_Rect m_default_rect;
        /* top of the rendered folder tree */
        int m_top_y;

        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_opened_folder_texture;
        std::unique_ptr<SDL_Texture, common::TextureDeleter> m_closed_folder_texture;
    };
}