#include "SceneConfig.h"
#include "FileSystem.h"
#include "Logger.h"

namespace engine {

SceneConfig::SceneConfig(const std::filesystem::path& path) :
    m_path(path)
{
    loadConfig(path);
}

auto SceneConfig::id() const -> uint32_t
{
    return m_id;
}

auto SceneConfig::name() const -> std::string
{
    return m_name;
}

auto SceneConfig::document() const -> const rapidjson::Document&
{
    return m_document;
}

auto SceneConfig::path() const -> const std::filesystem::path&
{
    return m_path;
}

void SceneConfig::loadConfig(const std::filesystem::path& path)
{
    if (!FileSystem::exists(path) || !FileSystem::isFile(path)) {
        Logger::error("SceneConfig::loadConfig() File not found: {}", path.string());
        return;
    }

    auto file = FileSystem::file(path, std::ios::in);
    auto text = file.readText();

    m_document.Parse(text.c_str());

    m_id = m_document["id"].GetUint();
    m_name = m_document["name"].GetString();
}

}