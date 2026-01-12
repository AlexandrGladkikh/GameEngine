#pragma once

#include <rapidjson/document.h>
#include <rapidjson/pointer.h>

#include <filesystem>
#include <string>
#include <string_view>

namespace engine {

class EngineSettings {
public:
    explicit EngineSettings(const std::filesystem::path& path);
    ~EngineSettings() = default;
    EngineSettings(const EngineSettings&) = delete;
    EngineSettings(EngineSettings&&) = delete;
    EngineSettings& operator=(const EngineSettings&) = delete;
    EngineSettings& operator=(EngineSettings&&) = delete;

    bool load();
    void save() const;

    auto settings() const -> const rapidjson::Document&;

private:
    std::filesystem::path m_path;
    rapidjson::Document m_document;
};

}
