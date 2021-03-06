#pragma once
#include "plugin/plugin.h"
#include <vector>
#include <string>


class PluginManager
{
public:
    PluginManager(const std::vector<std::string>& plugin_paths);

    void run_plugins();
    void run_plugin(Plugin* plugin);

    
    plugin::Node* visit(plugin::Node* node);

    plugin::Node* visit_variable_definition(plugin::Node* node);
    plugin::Node* visit_variable(plugin::Node* node);

    plugin::Node* visit_compound(plugin::Node* node);

    plugin::Node* visit_function_call(plugin::Node* node);

    plugin::Node* get_variable_from_name(const std::string& variable_name);

    std::vector<Plugin*> get_plugins_containing_variable(const std::string& variable_name);


private:
    std::vector<std::unique_ptr<Plugin>> m_plugins; 
    Plugin* m_current_loading_plugin{ 0 };
};
