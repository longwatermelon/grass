#pragma once
#include <string>
#include <filesystem>
#include <SDL.h>

namespace fs = std::filesystem;


namespace gui
{
    enum class ExplorerMode
    {
        FILE,
        FOLDER
    };

    class Explorer
    {
    public:
        Explorer(const std::string& path, ExplorerMode mode);

        void mainloop();

    private:
        SDL_Window* m_window{ nullptr };
        SDL_Renderer* m_rend{ nullptr };

        ExplorerMode m_mode;
        std::string m_current_path;
    };
}