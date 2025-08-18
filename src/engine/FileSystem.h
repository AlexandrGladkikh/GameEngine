#pragma once

#include <list>
#include <filesystem>
#include <fstream>
#include <vector>

namespace engine {

class FileSystem final {
public:
    class Directory final {
    public:
        Directory(const std::filesystem::path& path);
        ~Directory() = default;
        Directory(const Directory&) = delete;
        Directory(Directory&&) = delete;
        Directory& operator=(const Directory&) = delete;
        Directory& operator=(Directory&&) = delete;

        [[nodiscard]]
        auto ls() const -> std::list<std::filesystem::path>;
        void cd(const std::filesystem::path& path);
        auto path() const -> std::filesystem::path;

    private:
        std::filesystem::path m_path;
    };

    class File final {
    public:
        explicit File(const std::filesystem::path& path, std::ios::openmode type);
        ~File();
        File(const File&) = delete;
        File(File&&) = delete;
        File& operator=(const File&) = delete;
        File& operator=(File&&) = delete;

        auto readBinary() const -> std::vector<uint8_t>;
        auto readText() const -> std::string;

        auto writeBinary(const std::vector<uint8_t>& data) const -> bool;
        auto writeText(const std::string& data) const -> bool;

    private:
        mutable std::fstream m_file;
    };

    static bool exists(const std::filesystem::path& path);
    static bool isDirectory(const std::filesystem::path& path);
    static bool isFile(const std::filesystem::path& path);
    static bool removeFile(const std::filesystem::path& path);

    static auto directory(const std::filesystem::path& path) -> Directory;
    static auto file(const std::filesystem::path& path, std::ios::openmode type) -> File;
};

}
