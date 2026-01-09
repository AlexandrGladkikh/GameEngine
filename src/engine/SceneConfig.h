#pragma once

#include <rapidjson/document.h>

#include <string>
#include <cstdint>
#include <filesystem>

namespace engine {

class SceneConfig final
{
public:
    explicit SceneConfig(const std::filesystem::path& path);
    ~SceneConfig() = default;
    SceneConfig(const SceneConfig&) = delete;
    SceneConfig(SceneConfig&&) = delete;
    SceneConfig& operator=(const SceneConfig&) = delete;
    SceneConfig& operator=(SceneConfig&&) = delete;

    auto id() const -> uint32_t;
    auto name() const -> std::string;
    auto document() const -> const rapidjson::Document&;
    auto path() const -> const std::filesystem::path&;

private:
    void loadConfig(const std::filesystem::path& path);

    uint32_t m_id = -1;
    std::string m_name;
    std::filesystem::path m_path;

    rapidjson::Document m_document = rapidjson::Document();
};

}
