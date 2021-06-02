#pragma once
#include "text.h"
#include <string>
#include <vector>


namespace gui
{
    class File
    {
    public:
        File() = default;
        File(const std::string& base_path, const Text& name, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, int offset);

        void update_rect(SDL_Rect& r) { m_rect = r; r.y += m_name.char_dim().y; }

        std::string path();
        Text name() const { return m_name; }
        SDL_Rect rect() const { return m_rect; }

        void reset_rect() { m_rect = { -1, -1, -1, -1 }; }

    private:
        SDL_Rect m_rect;

        std::string m_base_path;
        Text m_name;

        SDL_Texture* m_tex;
    };

    class Folder
    {
    public:
        Folder(const std::string& base_path, const Text& name, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, int offset, SDL_Texture* closed_tex, SDL_Texture* opened_tex);

        /* If already collapsed, folder will expand. */
        void collapse();

        void update_rects(SDL_Rect& rect);

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

        SDL_Texture* m_tex;

        bool m_collapsed{ false };
    };

    class Tree
    {
    public:
        Tree(const Folder& folder, SDL_Rect starting_rect, SDL_Renderer* rend);

        void render(SDL_Renderer* rend);

        File* check_file_click(Folder& folder, int mx, int my);
        Folder* check_folder_click(Folder& folder, int mx, int my);

        void collapse_folder(Folder& folder);

        void update_display();

        Folder& folder() { return m_folder; }

    private:
        Folder m_folder;
        SDL_Rect m_default_rect;

        SDL_Texture* m_opened_folder_texture;
        SDL_Texture* m_closed_folder_texture;
    };
}