#include "grass.h"
#include <iostream>
#include <filesystem>
#include <SDL_image.h>

namespace fs = std::filesystem;


int main(int argc, char** argv)
{
#ifdef NDEBUG
    // not debugging
    std::string exe_dir = fs::absolute(argv[0]).parent_path().string();
#else
    // debugging
    std::string exe_dir = fs::current_path().string();
#endif

    Grass g(exe_dir);
    g.mainloop();

    return 0;
}