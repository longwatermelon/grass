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
    plugin::Parser parser(plugin_path); 

    try
    {
        m_ast = parser.parse();
    }
    catch (std::runtime_error& ex)
    {
        std::cout << "\033[31merror:\033[0m " << fs::absolute(fs::path(plugin_path).lexically_normal()).string() << ": " <<  ex.what() << "\n";
        m_ast = 0;
    }
}

