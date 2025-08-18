#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>

namespace engine {

class Context;
class SceneTransition;
class UserComponentsBuilder;
class Engine;

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

    auto context() const -> std::shared_ptr<Context>;

    auto getActiveSceneId() const -> uint32_t;

    bool saveScene(uint32_t id);

private:
    bool m_run = false;

    std::shared_ptr<Context> m_context;

    std::unique_ptr<SceneTransition> m_sceneTransition;

    std::unordered_map<uint32_t, std::filesystem::path> m_scenes_info;

    uint32_t m_active_scene_id = 0;

    friend class EngineAccessor;
};

}
