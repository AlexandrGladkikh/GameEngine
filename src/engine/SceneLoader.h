#pragma once

#include <filesystem>
#include <memory>
#include <optional>

namespace engine {

class Scene;
struct Context;
class SceneConfig;

class SceneLoader final {
public:
    static auto loadScene(const std::shared_ptr<Context>& context, const std::shared_ptr<SceneConfig>& scene_config) -> std::optional<std::unique_ptr<Scene>>;
};

}
