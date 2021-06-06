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
        DIR
    };

    class Explorer
    {
    public:
        Explorer(const std::string& dir, ExplorerMode mode, SDL_Point pos);
        ~Explorer();

        std::string get_path();

        void cleanup(std::vector<Button*>& buttons, TTF_Font** font);

        void update_current_directory();
        void render_current_directory(TTF_Font* font, SDL_Point font_dim);

        std::string elem_at_mouse_pos(int my, int font_dim_y);
        void highlight_elem_at_mouse(int my, int font_dim_y);

    private:
        SDL_Window* m_window{ nullptr };
        SDL_Renderer* m_rend{ nullptr };

        ExplorerMode m_mode;
        std::string m_current_dir;
        std::string m_selected_item;

        std::vector<std::string> m_current_names;
        std::vector<SDL_Texture*> m_current_textures;
    };
}