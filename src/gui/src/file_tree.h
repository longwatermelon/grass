#pragma once
#include "text.h"
#include <string>
#include <vector>


namespace gui
{
    class File
    {
    public:
        File(const std::string& base_path, const Text& name, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, SDL_Rect rect, int offset);

        Text name() const { return m_name; }

    private:
        std::string m_base_path;
        Text m_name;

        SDL_Texture* m_tex;
    };

    class Folder
    {
    public:
        Folder(const std::string& base_path, const Text& name, SDL_Renderer* rend);

        void render(SDL_Renderer* rend, SDL_Rect& rect, int offset);

        std::vector<File> files() const { return m_files; }
        std::vector<Folder> folders() const { return m_folders; }
        Text name() const { return m_name; }

    private:
        std::string m_base_path;
        Text m_name;

        std::vector<File> m_files;
        std::vector<Folder> m_folders;

        SDL_Texture* m_tex;
    };

    class Tree
    {
    public:
        Tree(const Folder& folder, SDL_Rect starting_rect, SDL_Color bg_color);

        void render(SDL_Renderer* rend);

    private:
        Folder m_folder;
        SDL_Rect m_default_rect;
        SDL_Color m_bg_color;
    };
}