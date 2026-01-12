#include "EngineSettings.h"

#include "FileSystem.h"
#include "Logger.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

#include <string>

namespace engine {

EngineSettings::EngineSettings(const std::filesystem::path& path) :
    m_path(path)
{

}

bool EngineSettings::load()
{
    if (!FileSystem::exists(m_path) || !FileSystem::isFile(m_path)) {
        Logger::error("EngineSettings::load() File not found: {}", m_path.string());
        return false;
    }

    auto file = FileSystem::file(m_path, std::ios::in);
    auto text = file.readText();

    m_document.Parse(text.c_str());

    if (m_document.HasParseError()) {
        Logger::error("EngineSettings::load() Parse error in file: {}", m_path.string());
        return false;
    }
    if (!m_document.IsObject()) {
        Logger::error("EngineSettings::load() Root JSON value must be object: {}", m_path.string());
        return false;
    }

    return true;
}

void EngineSettings::save() const
{
    if (!FileSystem::exists(m_path) || !FileSystem::isFile(m_path)) {
        Logger::error("EngineSettings::save() File not found: {}", m_path.string());
        return;
    }

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    m_document.Accept(writer);

    auto file = FileSystem::file(m_path, std::ios::out | std::ios::trunc);
    file.writeText(buffer.GetString());
}

auto EngineSettings::settings() const -> const rapidjson::Document&
{
    return m_document;
}

}