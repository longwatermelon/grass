#include "plugin_manager.h"
#include <iostream>


PluginManager::PluginManager(const std::vector<std::string>& plugin_paths)
{
    for (int i = 0; i < plugin_paths.size(); ++i)
    {
        m_plugins.emplace_back(Plugin(plugin_paths[i]));
        
        if (m_plugins[m_plugins.size() - 1].ast())
            std::cout << "Loaded plugin " << i + 1 << " of " << plugin_paths.size() << "\n";
        else
            std::cout << "\033[31mFailed to load plugin " << i + 1 << " of " << plugin_paths.size() << "\033[0m\n";
    }
}

