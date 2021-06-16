#pragma once
#include "plugin.h"
#include <vector>
#include <string>


class PluginManager
{
public:
    PluginManager(const std::vector<std::string>& plugin_paths);

private:
    std::vector<Plugin> m_plugins; 
};
