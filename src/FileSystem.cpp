#include "FileSystem.h"

namespace engine {

FileSystem::Directory::Directory(const std::filesystem::path& path) :
    m_path(path)
{

}

std::list<std::filesystem::path> FileSystem::Directory::ls() const
{
    std::list<std::filesystem::path> result;
    for (auto& p : std::filesystem::directory_iterator(m_path)) {
        result.push_back(p.path());
    }
    return result;
}

void FileSystem::Directory::cd(const std::filesystem::path& path)
{
    m_path /= path;
}

std::filesystem::path FileSystem::Directory::path() const
{
    return m_path;
}

FileSystem::File::File(const std::filesystem::path& path, std::ios::openmode type)
{
    m_file.open(path, std::ios::out | type);
    m_file.seekg(0, std::ios::beg);
}


FileSystem::File::~File()
{
    m_file.close();
}

auto FileSystem::File::readBinary() const -> std::vector<uint8_t>
{
    m_file.seekg(0, std::ios::end);
    std::vector<uint8_t> result(m_file.tellg());
    m_file.seekg(0, std::ios::beg);
    m_file.read(reinterpret_cast<char*>(result.data()), result.size());
    return result;
}

auto FileSystem::File::readText() const -> std::string
{
    m_file.seekg(0, std::ios::end);
    std::string result(m_file.tellg(), ' ');
    m_file.seekg(0, std::ios::beg);
    m_file.read(&result[0], result.size());
    return result;
}

bool FileSystem::exists(const std::filesystem::path& path)
{
    return std::filesystem::exists(path);
}

bool FileSystem::isDirectory(const std::filesystem::path& path)
{
    return std::filesystem::is_directory(path);
}

bool FileSystem::isFile(const std::filesystem::path& path)
{
    return std::filesystem::is_regular_file(path);
}

auto FileSystem::directory(const std::filesystem::path &path)
{
    return Directory(path);
}

auto FileSystem::file(const std::filesystem::path &path, std::ios::openmode type)
{
    return File(path, type);
}

}
