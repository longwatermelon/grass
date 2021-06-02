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

        void render(SDL_Renderer* rend, SDL_Rect rect, int offset);

        std::string path();
        Text name() const { return m_name; }
        SDL_Rect rect() const { return m_rect; }

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

        void render(SDL_Renderer* rend, SDL_Rect& rect, int offset);

        std::vector<File>& files() { return m_files; }
        std::vector<Folder>& folders() { return m_folders; }
        Text name() const { return m_name; }
        SDL_Rect rect() const { return m_rect; }

    private:
        SDL_Rect m_rect;

        std::string m_base_path;
        Text m_name;

        std::vector<File> m_files;
        std::vector<Folder> m_folders;

        SDL_Texture* m_tex;
    };

    class Tree
    {
    public:
        Tree(const Folder& folder, SDL_Rect starting_rect);

        void render(SDL_Renderer* rend);

        File* check_click(int mx, int my);
        File* check_click(Folder& folder, int mx, int my);

    private:
        Folder m_folder;
        SDL_Rect m_default_rect;
    };
}