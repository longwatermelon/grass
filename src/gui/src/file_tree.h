#pragma once
#include "text.h"
#include <string>
#include <vector>


namespace gui
{
    class File
    {
    public:
        File(const std::string& base_path, const Text& name);

    private:
        std::string m_base_path;
        Text m_name;
    };

    class Folder
    {
    public:
        Folder(const std::string& base_path, const Text& name);

    private:
        std::string m_base_path;
        Text m_name;

        std::vector<File> m_files;
        std::vector<Folder> m_folders;
    };
}