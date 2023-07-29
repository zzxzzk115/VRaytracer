#pragma once

#include "Base.h"

namespace VRaytracer
{
    class FileSystem
    {
    public:
        static void InitExecutableDirectory(const char* executableFilePath)
        {
            std::filesystem::path exe_path(executableFilePath);
            s_ExeDirectory = exe_path.parent_path();
        }

        static const std::filesystem::path& GetExecutableDirectory() { return s_ExeDirectory; }
        static std::filesystem::path        GetExecutableRelativeDirectory(const std::filesystem::path& path)
        {
            return s_ExeDirectory / path;
        }

        static bool Exists(const std::string& path) { return std::filesystem::exists(path); }
        static bool Exists(const std::filesystem::path& path) { return std::filesystem::exists(path); }

        static std::string GetFileName(const std::string& filePath)
        {
            return std::filesystem::path(filePath).filename().string();
        }

        static bool WriteAllText(const std::string& path, const std::string& content)
        {
            std::ofstream out(path);
            if (out.bad())
            {
                return false;
            }

            out << content;

            out.close();
            return true;
        }

        static std::string ReadAllText(const std::string& path)
        {
            std::ifstream in(path);

            if (in.bad())
            {
                return std::string();
            }

            std::stringstream ss;
            ss << in.rdbuf();

            return std::string(ss.str());
        }

    private:
        static std::filesystem::path s_ExeDirectory;
    };
}; // namespace VRaytracer