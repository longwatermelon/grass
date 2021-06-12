#include "grass.h"
#include <iostream>
#include <filesystem>
#include <SDL_image.h>

namespace fs = std::filesystem;


int main(int argc, char** argv)
{
    std::string exe_dir = fs::current_path().string();

    Grass g(exe_dir);
    g.mainloop();

    return 0;
}
