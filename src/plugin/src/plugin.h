#pragma once
#include "detail/parser.h"
#include <string>
#include <memory>


class Plugin
{
public:
    Plugin() = default;
    Plugin(const std::string& plugin_path); 

    plugin::Node* ast() { return m_ast.get(); }
    
private:
    std::unique_ptr<plugin::Node> m_ast; 
};
