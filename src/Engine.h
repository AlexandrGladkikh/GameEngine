#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>

namespace engine {

class Context;
class SceneTransition;

class Engine final {
public:
    explicit Engine();
    ~Engine();

    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

    bool initialize(const std::filesystem::path& config_path);

    void run();

private:
    std::shared_ptr<Context> m_context;

    std::unique_ptr<SceneTransition> m_sceneTransition;

    std::unordered_map<uint32_t, std::filesystem::path> m_scenesInfo;

    uint32_t m_active_scene_id = 0;
};

}
