#pragma once
#include "text_entry.h"


class Grass
{
public:
    Grass(const std::string& exe_dir);
    ~Grass();

public:
    void mainloop();

    void load_file(const std::string& fp, gui::TextEntry& entry);
    void reset_entry_to_default(gui::TextEntry& entry);

private:
    SDL_Window* m_window;
    SDL_Renderer* m_rend;

    gui::TextEntry* m_selected_entry{ nullptr };

    std::string m_exe_dir;
};