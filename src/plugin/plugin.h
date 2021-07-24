#pragma once
#include "detail/parser.h"
#include <string>
#include <memory>


class Plugin
{
public:
    Plugin() = default;
    Plugin(const std::string& plugin_path);

    plugin::Node* variable_from_name(const std::string& name);

    plugin::Node* ast() { return m_ast.get(); }
    std::string name() { return m_name; }
    void add_variable(plugin::Node* var) { m_variables.emplace_back(var); }
    std::vector<plugin::Node*> variables() { return m_variables; }
    
private:
    std::string m_name;

    std::unique_ptr<plugin::Node> m_ast; 
    std::vector<plugin::Node*> m_variables;
};

