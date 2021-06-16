#pragma once
#include "plugin.h"
#include <vector>
#include <string>


class PluginManager
{
public:
    PluginManager(const std::vector<std::string>& plugin_paths);

    void run_plugins();
    void run_plugin(Plugin& plugin);

    
    plugin::Node* visit(plugin::Node* node);

    plugin::Node* visit_variable_definition(plugin::Node* node);
    plugin::Node* visit_variable(plugin::Node* node);

    plugin::Node* visit_compound(plugin::Node* node);

    plugin::Node* visit_function_call(plugin::Node* node);


private:
    std::vector<Plugin> m_plugins; 

    std::vector<plugin::Node*> m_variable_definitions;
};
