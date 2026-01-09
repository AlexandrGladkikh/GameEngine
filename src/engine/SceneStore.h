#pragma once

#include <memory>
#include <optional>
#include <unordered_map>
#include <string>

namespace engine {

class Scene;

class SceneStore final {
public:
    SceneStore() = default;
    ~SceneStore() = default;
    SceneStore(const SceneStore&) = delete;
    SceneStore(SceneStore&&) = delete;
    SceneStore& operator=(const SceneStore&) = delete;
    SceneStore& operator=(SceneStore&&) = delete;

    auto get(uint32_t id) const -> std::optional<std::shared_ptr<Scene>>;
    auto get(const std::string& name) const -> std::optional<std::shared_ptr<Scene>>;
    void add(uint32_t id, std::shared_ptr<Scene> scene);
    void remove(uint32_t id);
    auto getAll() const -> std::unordered_map<uint32_t, std::shared_ptr<Scene>>;

private:
    std::unordered_map<uint32_t, std::shared_ptr<Scene>> m_scenes;
};

}
