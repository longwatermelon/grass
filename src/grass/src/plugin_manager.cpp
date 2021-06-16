#include "plugin_manager.h"
#include <iostream>


PluginManager::PluginManager(const std::vector<std::string>& plugin_paths)
{
    for (int i = 0; i < plugin_paths.size(); ++i)
    {
        m_plugins.emplace_back(Plugin(plugin_paths[i]));
        std::cout << "Loaded plugin " << i + 1 << " of " << plugin_paths.size() << "\n";
    }
}

