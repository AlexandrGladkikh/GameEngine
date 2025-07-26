#pragma once

#include <memory>
#include <filesystem>
#include <unordered_map>

namespace engine {

class Context;

class Engine final {
public:
    explicit Engine();
    ~Engine();

    Engine(const Engine&) = delete;
    Engine(Engine&&) = delete;
    Engine& operator=(const Engine&) = delete;
    Engine& operator=(Engine&&) = delete;

    bool initialize(const std::filesystem::path& config_path);

private:
    std::shared_ptr<Context> m_context;

    std::unordered_map<uint32_t, std::filesystem::path> m_scenesInfo;
};

}
