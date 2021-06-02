#include "file_tree.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;


gui::File::File(const std::string& base_path, const Text& name)
    : m_base_path(base_path), m_name(name) {}


gui::Folder::Folder(const std::string& base_path, const Text& name)
    : m_base_path(base_path), m_name(name)
{
    Text t = name;
    std::string sname = m_name.str();

    for (auto& entry : fs::directory_iterator(m_base_path + "\\" + sname))
    {
        t.set_contents({ entry.path().filename().string() });
        
        if (entry.is_directory())
        {
            m_folders.emplace_back(Folder(m_base_path + (sname.empty() ? "" : "\\" + sname), t));
        }
        else
        {
            m_files.emplace_back(File(m_base_path + (sname.empty() ? "" : "\\" + sname), t));
        }
    }
}