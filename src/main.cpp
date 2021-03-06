#include "grass.h"
#include "plugin_manager.h"
#include <iostream>
#include <filesystem>
#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#else
#include <unistd.h>
#include <libgen.h>
#include <linux/limits.h>
#endif
#include <SDL_image.h>

namespace fs = std::filesystem;


int main(int argc, char** argv)
{
#ifdef NDEBUG
#ifdef _WIN32
    std::string exe_dir;
    TCHAR path[FILENAME_MAX];
    GetModuleFileName(nullptr, path, FILENAME_MAX);
    exe_dir = fs::path(path).parent_path().string();
#else
    std::string exe_dir = "/usr/share/grass";
#endif // ifdef _WIN32
#else
    std::string exe_dir = fs::current_path().string();
#endif // ifdef NDEBUG

    Grass g(exe_dir);
    g.mainloop();

    return 0;
}
