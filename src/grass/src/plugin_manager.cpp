#include "plugin_manager.h"
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

