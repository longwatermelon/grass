#pragma once
#include "button.h"
#include <string>
#include <filesystem>
#include <vector>

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
        Explorer(const std::string& path, ExplorerMode mode, SDL_Point pos);

        void mainloop();

        void cleanup(std::vector<Button*>& buttons, TTF_Font** font);

    private:
        SDL_Window* m_window{ nullptr };
        SDL_Renderer* m_rend{ nullptr };

        ExplorerMode m_mode;
        std::string m_current_path;
    };
}