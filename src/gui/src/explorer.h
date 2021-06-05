#pragma once
#include <string>
#include <filesystem>

namespace fs = std::filesystem;


namespace gui
{
    enum class ExplorerMode
    {
        FILE,
        FOLDER
    };

    class Explorer
    {
    public:
        Explorer(const std::string& path, ExplorerMode mode);

    private:
        ExplorerMode m_mode;
    };
}