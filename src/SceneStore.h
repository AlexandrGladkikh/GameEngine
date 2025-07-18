#pragma once

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

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

    auto get(const std::string& id) const -> std::optional<std::shared_ptr<Scene>>;
    void add(const std::string& id, std::unique_ptr<Scene> scene);

private:
    std::unordered_map<std::string, std::shared_ptr<Scene>> m_scenes;
};

}
