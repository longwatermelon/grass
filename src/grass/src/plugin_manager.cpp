#include "plugin_manager.h"
#include <sstream>
#include <iostream>


PluginManager::PluginManager(const std::vector<std::string>& plugin_paths)
{
    for (int i = 0; i < plugin_paths.size(); ++i)
    {
        m_plugins.emplace_back(Plugin(plugin_paths[i]));
        
        if (m_plugins[m_plugins.size() - 1].ast())
            std::cout << "Loaded plugin '" << plugin_paths[i] << "': " << i + 1 << "/" << plugin_paths.size() << "\n";
    }
}


void PluginManager::run_plugins()
{
    for (auto& plugin : m_plugins)
    {
        run_plugin(plugin);
    }
}


void PluginManager::run_plugin(Plugin& plugin)
{
    plugin::Node* root = plugin.ast();
    m_current_loading_plugin = &plugin;
    visit(root);
}


plugin::Node* PluginManager::visit(plugin::Node* node)
{
    if (!node) return node;

    switch (node->type)
    {
        case plugin::NodeType::VARIABLE_DEFINITION: return visit_variable_definition(node);
        case plugin::NodeType::VARIABLE: return visit_variable(node);
        case plugin::NodeType::FUNCTION_CALL: return visit_function_call(node);
        case plugin::NodeType::STRING: return node;
        case plugin::NodeType::INT: return node;
        case plugin::NodeType::COMPOUND: return visit_compound(node);
        case plugin::NodeType::NOOP: return node;
    }
    
    throw std::runtime_error("Uncaught statement of type " + std::to_string((int)node->type));
}


plugin::Node* PluginManager::visit_variable_definition(plugin::Node* node)
{
    m_current_loading_plugin->add_variable(node);
    return node;
}


plugin::Node* PluginManager::visit_variable(plugin::Node* node)
{
    for (auto& def : m_current_loading_plugin->variables())
    {
        if (def->variable_definition_name == node->variable_name)
            return visit(def->variable_definition_value.get());
    }


    std::stringstream ss;
    ss << "Undefined variable '" << node->variable_name << "'";
    throw std::runtime_error(ss.str());
}


plugin::Node* PluginManager::visit_compound(plugin::Node* node)
{
    for (auto& n : node->compound_value)
    {
        visit(n.get());
    }

    return 0;
}


plugin::Node* PluginManager::visit_function_call(plugin::Node* node)
{
    std::stringstream ss;
    ss << "Undefined function '" << node->function_call_name << "'";
    throw std::runtime_error(ss.str());
}


plugin::Node* PluginManager::get_variable_from_name(const std::string& variable_name)
{
    for (auto& plugin : m_plugins)
    {
        plugin::Node* var = plugin.variable_from_name(variable_name);

        if (var)
            return visit(var->variable_definition_value.get());
    }

    return 0;
}

