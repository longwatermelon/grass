#include "plugin.h"
#include "detail/lexer.h"
#include "detail/parser.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;


Plugin::Plugin(const std::string& plugin_path)
{
    std::ifstream ifs(plugin_path);

    std::stringstream ss;
    std::string line;
    while (std::getline(ifs, line)) ss << line << "\n";

    ifs.close();

    plugin::Parser parser(plugin_path); 

    try
    {
        m_ast = parser.parse();
    }
    catch (std::runtime_error& ex)
    {
        std::cout << fs::absolute(fs::path(plugin_path).lexically_normal()).string() << ": " <<  ex.what() << "\n";
    }
}

