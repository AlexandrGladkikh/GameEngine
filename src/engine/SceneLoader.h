#pragma once

#include <filesystem>
#include <memory>
#include <optional>

namespace engine {

class Scene;
class Context;

class SceneLoader final {
public:
    static auto loadScene(const std::shared_ptr<Context>& context, const std::filesystem::path& path) -> std::optional<std::unique_ptr<Scene>>;
};

}
