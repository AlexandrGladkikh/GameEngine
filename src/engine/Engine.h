#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>

namespace engine {

struct Context;
class SceneTransition;
class UserComponentsBuilder;
class Engine;
class SceneConfig;
class EngineSettings;

class EngineAccessor final {
public:
    explicit EngineAccessor(Engine& engine);
    ~EngineAccessor() = default;
    EngineAccessor(const EngineAccessor&) = delete;
    EngineAccessor(EngineAccessor&&) = delete;
    EngineAccessor& operator=(const EngineAccessor&) = delete;
    EngineAccessor& operator=(EngineAccessor&&) = delete;

    void stop() const;

private:
    Engine& m_engine;
};

class Engine final {
public:
    explicit Engine();
    ~Engine();

    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

    bool initialize(const std::filesystem::path& config_path);

    void setUserComponentsBuilder(std::unique_ptr<UserComponentsBuilder> userComponentsBuilder);

    void run();

    void performTick(uint64_t dt);

    auto context() const -> std::shared_ptr<Context>;

    auto getActiveSceneId() const -> uint32_t;

    auto getScenesInfo() const -> std::unordered_map<uint32_t, std::shared_ptr<SceneConfig>>;

    auto getEngineSettings() const -> std::shared_ptr<EngineSettings>;

    bool saveScene(uint32_t id);

    uint32_t addEmptyScene(const std::string& name, const std::filesystem::path& path);

    bool changeActiveScene(uint32_t id);

    void needReloadResourcePackage(uint32_t id);

private:
    void prepareTick();
    void tick(uint64_t dt);

    bool m_run = false;

    std::shared_ptr<Context> m_context;

    std::unique_ptr<SceneTransition> m_sceneTransition;

    std::unordered_map<uint32_t, uint32_t> m_engine_config_scene_id_to_scene_id;

    std::unordered_map<uint32_t, std::shared_ptr<SceneConfig>> m_scenes_info;

    std::shared_ptr<EngineSettings> m_engine_settings;

    uint32_t m_active_scene_id = 0;

    bool m_need_reload_resource_package = false;
    uint32_t m_reload_resource_package_id = -1;

    friend class EngineAccessor;
};

}
