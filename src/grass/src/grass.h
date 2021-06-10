#pragma once
#include "text_entry.h"
#include "basic_text_entry.h"
#include "file_tree.h"
#include "scrollbar.h"
#include "menu.h"
#include "button.h"
#include <memory>


class Grass
{
public:
    Grass(const std::string& exe_dir);
    ~Grass();

public:
    void mainloop();

    void load_file(const std::string& fp, gui::TextEntry& entry);
    void reset_entry_to_default(gui::TextEntry& entry);

    void handle_mouse_down(Uint8 button, bool& mouse_down, int mx, int my, gui::Menu* menu, std::string& current_open_fp, SDL_Texture* editor_image);
    void handle_mouse_up(bool& mouse_down);

    void handle_textinput(char c, std::string& current_open_fp);

private:
    SDL_Window* m_window;
    SDL_Renderer* m_rend;

    gui::TextEntry* m_selected_entry{ nullptr };
    gui::BasicTextEntry* m_selected_basic_entry{ nullptr };

    std::string m_exe_dir;

    gui::common::Font m_font_textbox;
    gui::common::Font m_font_tree;

    std::vector<gui::TextEntry> m_text_entries;
    std::vector<gui::BasicTextEntry> m_basic_text_entries;

    gui::Tree* m_tree{ nullptr };

    gui::Scrollbar m_scrollbar;

    std::vector<std::unique_ptr<gui::Button>> m_buttons;
};