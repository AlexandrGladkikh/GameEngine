#include "Engine.h"

#include "Context.h"
#include "Window.h"
#include "MeshStore.h"
#include "ShaderStore.h"
#include "TextureStore.h"
#include "ResourcePackageStore.h"
#include "SceneStore.h"
#include "FileSystem.h"
#include "SceneTransition.h"
#include "Renderer.h"
#include "Logger.h"

#include <rapidjson/document.h>

namespace engine {

std::optional<std::unique_ptr<Window>> configWindow(const rapidjson::Value& value)
{
    Logger::info(__FUNCTION__);

    GLint window_width = value["width"].GetInt();
    GLint window_height = value["height"].GetInt();
    std::string window_title = value["title"].GetString();

    auto window = std::make_unique<Window>(window_title, window_width, window_height);

    return window;
}

bool configScenesInfo(const rapidjson::Value& value, std::unordered_map<uint32_t, std::filesystem::path>& infos)
{
    Logger::info(__FUNCTION__);

    if (!value.IsArray()) {
        Logger::error("scenes must be an array");
        return false;
    }

    for (const auto& scene : value.GetArray()) {
        infos[scene["id"].GetUint()] = scene["path"].GetString();
    }

    return true;
}

Engine::Engine() :
    m_context(std::make_shared<Context>()),
    m_sceneTransition(std::make_unique<SceneTransition>(m_context))
{
    Logger::info(__FUNCTION__);

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
    Logger::info(__FUNCTION__);

    if (!FileSystem::exists(config_path) || !FileSystem::isFile(config_path)) {
        Logger::error("config file not found");
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

    m_context->resourcePackageStore->initResourcePackagesInformation(document["resource_packages"].GetString());

    if (!configScenesInfo(document["scenes"], m_scenesInfo)) {
        return false;
    }

    m_active_scene_id = document["main_scene_id"].GetUint();

    return m_sceneTransition->transition(m_scenesInfo, -1, m_active_scene_id);
}

void Engine::run()
{
    GLdouble update_time = glfwGetTime();
    uint64_t delta_time = 0;

    while (true) {
        delta_time = (glfwGetTime() - update_time) * 1000000;
        update_time = glfwGetTime();
        Logger::info("delta time: {}", delta_time);

        m_context->window->update(delta_time);

        auto scene = m_context->sceneStore->get(m_active_scene_id);
        if (scene.has_value()) {
            Renderer::render(m_context, scene.value());
        }

        m_context->window->swapBuffer();
    }
}

}
