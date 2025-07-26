#include "Engine.h"

#include "Context.h"
#include "Window.h"
#include "MeshStore.h"
#include "ShaderStore.h"
#include "TextureStore.h"
#include "ResourcePackageStore.h"
#include "SceneStore.h"
#include "FileSystem.h"

#include <rapidjson/document.h>

namespace engine {

std::optional<std::unique_ptr<Window>> configWindow(const rapidjson::Value& value) {
    GLint window_width = value["width"].GetInt();
    GLint window_height = value["height"].GetInt();
    std::string window_title = value["title"].GetString();

    auto window = std::make_unique<Window>(window_title, window_width, window_height);

    return window;
}

bool configScenesInfo(const rapidjson::Value& value, std::unordered_map<uint32_t, std::filesystem::path>& infos) {
    if (!value.IsArray()) {
        return false;
    }

    for (const auto& scene : value.GetArray()) {
        infos[scene["id"].GetUint()] = scene["path"].GetString();
    }

    return true;
}

Engine::Engine() :
        m_context(std::make_shared<Context>())
{
    m_context->meshStore = std::make_unique<MeshStore>();
    m_context->shaderStore = std::make_unique<ShaderStore>();
    m_context->textureStore = std::make_unique<TextureStore>();
    m_context->resourcePackageStore = std::make_unique<ResourcePackageStore>();
    m_context->sceneStore = std::make_unique<SceneStore>();
}

Engine::~Engine()
{

}

bool Engine::initialize(const std::filesystem::path& config_path)
{
    if (!FileSystem::exists(config_path) || !FileSystem::isFile(config_path)) {
        return false;
    }

    FileSystem::File config_file(config_path, std::ios::in);
    auto config_text = config_file.readText();

    rapidjson::Document document;
    document.Parse(config_text.c_str());

    auto window = configWindow(document["window"]);

    if (window.has_value()) {
        m_context->window = std::move(window.value());
    } else {
        return false;
    }

    if (!configScenesInfo(document["scenes"], m_scenesInfo)) {
        return false;
    }

    return true;
}

}
