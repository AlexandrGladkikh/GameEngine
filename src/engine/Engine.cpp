#include "Engine.h"

#include "Component.h"
#include "Context.h"
#include "FileSystem.h"
#include "InputManager.h"
#include "Logger.h"
#include "MeshStore.h"
#include "Node.h"
#include "Renderer.h"
#include "ResourcePackage.h"
#include "ResourcePackageStore.h"
#include "Scene.h"
#include "SceneStore.h"
#include "SceneTransition.h"
#include "ShaderStore.h"
#include "TextureStore.h"
#include "RenderPassStore.h"
#include "UserComponentsBuilder.h"
#include "Utils.h"
#include "Window.h"
#include "SceneConfig.h"
#include "EngineSettings.h"

#include <rapidjson/document.h>

#include <ranges>

namespace engine {

std::optional<std::unique_ptr<Window>> configWindow(const rapidjson::Value& value)
{
    Logger::debug(__FUNCTION__);

    GLint window_width = value["width"].GetInt();
    GLint window_height = value["height"].GetInt();
    std::string window_title = value["title"].GetString();
    GLint gl_version_major = value["gl_version"]["major"].GetInt();
    GLint gl_version_minor = value["gl_version"]["minor"].GetInt();

    auto window = std::make_unique<Window>(window_title, window_width, window_height, gl_version_major, gl_version_minor);

    return window;
}

bool configScenesInfo(const rapidjson::Value& value, std::unordered_map<uint32_t, std::shared_ptr<SceneConfig>>& infos, std::unordered_map<uint32_t, uint32_t>& engine_config_scene_id_to_scene_id)
{
    Logger::debug(__FUNCTION__);

    if (!value.IsArray()) {
        Logger::error("scenes must be an array");
        return false;
    }

    for (const auto& scene_info : value.GetArray()) {
        auto scene_config = std::make_shared<SceneConfig>(scene_info["path"].GetString());
        engine_config_scene_id_to_scene_id[scene_info["id"].GetUint()] = scene_config->id();
        infos[scene_config->id()] = scene_config;
    }

    return true;
}

EngineAccessor::EngineAccessor(Engine& engine) :
    m_engine(engine)
{

}

void EngineAccessor::stop() const
{
    m_engine.m_run = false;
}

Engine::Engine() :
    m_context(std::make_shared<Context>()),
    m_sceneTransition(std::make_unique<SceneTransition>(m_context))
{
    Logger::debug(__FUNCTION__);

    m_context->meshStore = std::make_unique<MeshStore>();
    m_context->shaderStore = std::make_unique<ShaderStore>();
    m_context->textureStore = std::make_unique<TextureStore>();
    m_context->resourcePackageStore = std::make_unique<ResourcePackageStore>();
    m_context->sceneStore = std::make_unique<SceneStore>();
    m_context->engineAccessor = std::make_unique<EngineAccessor>(*this);
    m_context->renderPassStore = std::make_unique<RenderPassStore>();
}

Engine::~Engine()
{

}

bool Engine::initialize(const std::filesystem::path& config_path)
{
    Logger::debug(__FUNCTION__);

    m_engine_settings = std::make_shared<EngineSettings>(config_path);
    if (!m_engine_settings->load()) {
        return false;
    }

    auto& settings = m_engine_settings->settings();

    auto log_level = std::string(settings["log_level"].GetString());
    if (log_level == "DEBUG") {
        Logger::setLogLevel(Level::DEBUG);
    } else if (log_level == "INFO") {
        Logger::setLogLevel(Level::INFO);
    } else if (log_level == "WARNING") {
        Logger::setLogLevel(Level::WARNING);
    } else if (log_level == "ERROR") {
        Logger::setLogLevel(Level::ERROR);
    } else {
        Logger::setLogLevel(Level::INFO);
        Logger::warning("Unknown log level: {}, using INFO", log_level);
    }

    auto window = configWindow(settings["window"]);

    if (window.has_value()) {
        m_context->window = std::move(window.value());
    } else {
        return false;
    }

    m_context->inputManager = std::make_unique<InputManager>(m_context->window);

    m_context->resourcePackageStore->initResourcePackagesInformation(settings["resource_packages"].GetString());

    if (!configScenesInfo(settings["scenes"], m_scenes_info, m_engine_config_scene_id_to_scene_id)) {
        return false;
    }

    m_active_scene_id = m_engine_config_scene_id_to_scene_id[settings["main_scene_id"].GetUint()];

    return m_sceneTransition->transition(m_scenes_info, -1, m_active_scene_id);
}

void Engine::setUserComponentsBuilder(std::unique_ptr<UserComponentsBuilder> userComponentsBuilder)
{
    m_context->userComponentsBuilder = std::move(userComponentsBuilder);
}

void Engine::run()
{
    m_run = true;

    GLdouble update_time = glfwGetTime();
    uint64_t delta_time = 0;

    while (m_run) {
        prepareTick();

        delta_time = (glfwGetTime() - update_time) * 1000000;
        update_time = glfwGetTime();

        tick(delta_time);
    }
}

void Engine::performTick(uint64_t dt)
{
    prepareTick();

    tick(dt);
}

auto Engine::context() const -> std::shared_ptr<Context>
{
    return m_context;
}

auto Engine::getActiveSceneId() const -> uint32_t
{
    return m_active_scene_id;
}

auto Engine::getScenesInfo() const -> std::unordered_map<uint32_t, std::shared_ptr<SceneConfig>>
{
    return m_scenes_info;
}

auto Engine::getEngineSettings() const -> std::shared_ptr<EngineSettings>
{
    return m_engine_settings;
}

bool Engine::saveScene(uint32_t id)
{
    auto scene_info = m_scenes_info.find(id);
    if (scene_info == m_scenes_info.end()) {
        return false;
    }

    auto scene = m_context->sceneStore->get(id);
    if (!scene.has_value()) {
        return false;
    }

    return saveSceneToFile(scene.value(), scene_info->second->path());
}

uint32_t Engine::addEmptyScene(const std::string& name, const std::filesystem::path& path)
{
    auto scene = std::make_shared<Scene>(m_context, generateUniqueId(), name);
    uint32_t id = scene->id();
    saveSceneToFile(scene, path);
    auto scene_config = std::make_shared<SceneConfig>(path);
    m_context->sceneStore->add(id, std::move(scene));
    m_scenes_info[scene_config->id()] = scene_config;
    return id;
}

bool Engine::changeActiveScene(uint32_t id)
{
    if (m_active_scene_id == id) {
        return true;
    }

    uint32_t old_active_scene = m_active_scene_id;
    m_active_scene_id = id;
    m_sceneTransition->transition(m_scenes_info, old_active_scene, m_active_scene_id);

    return true;
}

void Engine::needReloadResourcePackage(uint32_t id)
{
    m_need_reload_resource_package = true;
    m_reload_resource_package_id = id;
}

void Engine::prepareTick()
{
    if (m_need_reload_resource_package) {
        m_need_reload_resource_package = false;

        auto resource_package = m_context->resourcePackageStore->get(m_reload_resource_package_id);

        if (resource_package.has_value()) {
            loadResourcePackage(m_context, resource_package.value());
        }
    }
}

void Engine::tick(uint64_t dt)
{
    Logger::info("tick: {}", dt);

    m_context->window->update(dt);
    m_context->inputManager->update(dt);

    auto scene = m_context->sceneStore->get(m_active_scene_id);
    if (scene.has_value()) {
        for (auto& component : scene.value()->getComponents() | std::views::values) {
            if (component->getNode().value()->isActive() && component->isActive() && component->isValid()) {
                component->update(dt);
            }
        }

        Renderer::render(m_context, scene.value());
    }

    m_context->window->swapBuffer();
}

}
