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
#include "InputManager.h"
#include "Logger.h"
#include "Scene.h"
#include "Component.h"
#include "UserComponentsBuilder.h"
#include "Node.h"
#include "ResourcePackage.h"

#include <rapidjson/document.h>

#include <ranges>

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
    Logger::info(__FUNCTION__);

    m_context->meshStore = std::make_unique<MeshStore>();
    m_context->shaderStore = std::make_unique<ShaderStore>();
    m_context->textureStore = std::make_unique<TextureStore>();
    m_context->resourcePackageStore = std::make_unique<ResourcePackageStore>();
    m_context->sceneStore = std::make_unique<SceneStore>();
    m_context->engineAccessor = std::make_unique<EngineAccessor>(*this);
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

    m_context->inputManager = std::make_unique<InputManager>(m_context->window);

    m_context->resourcePackageStore->initResourcePackagesInformation(document["resource_packages"].GetString());

    if (!configScenesInfo(document["scenes"], m_scenes_info)) {
        return false;
    }

    m_active_scene_id = document["main_scene_id"].GetUint();

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
        if (m_need_reload_resource_package) {
            m_need_reload_resource_package = false;

            auto resource_package = m_context->resourcePackageStore->get(m_reload_resource_package_id);

            if (resource_package.has_value()) {
                loadResourcePackage(m_context, resource_package.value());
            }
        }

        delta_time = (glfwGetTime() - update_time) * 1000000;
        update_time = glfwGetTime();

        while (m_pause.load() != 0) {
            int expected = m_pause.load();
            m_on_pause.store(true);
            m_on_pause.notify_all();
            m_pause.wait(expected);
        }

        m_on_pause.store(false);
        m_on_pause.notify_all();

        Logger::info("delta time: {}", delta_time);

        m_context->window->update(delta_time);
        m_context->inputManager->update(delta_time);

        auto scene = m_context->sceneStore->get(m_active_scene_id);
        if (scene.has_value()) {
            for (auto& component : scene.value()->getComponents() | std::views::values) {
                if (component->getNode().value()->isActive() && component->isActive() && component->isValid()) {
                    component->update(delta_time);
                }
            }

            Renderer::render(m_context, scene.value());
        }

        m_context->window->swapBuffer();
    }
}

void Engine::pause()
{
    m_pause.fetch_add(1);
    m_pause.notify_all();

    m_on_pause.wait(false);
}

void Engine::resume()
{
    m_pause.fetch_sub(1);
    m_pause.notify_all();

    m_on_pause.wait(true);
}

auto Engine::context() const -> std::shared_ptr<Context>
{
    return m_context;
}

auto Engine::getActiveSceneId() const -> uint32_t
{
    return m_active_scene_id;
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

    return saveSceneToFile(scene.value(), scene_info->second);
}

void Engine::needReloadResourcePackage(uint32_t id)
{
    m_need_reload_resource_package = true;
    m_reload_resource_package_id = id;
}

}
