#pragma once

#include <memory>
#include <vector>
#include <unordered_map>

namespace engine {

class Node;
class Scene;
class Context;

enum class ComponentType {
    Transform,
    Mesh,
    Material,
    Camera
};

class SceneRequester final {
public:
    class SceneSlice final {
    public:
        explicit SceneSlice(const std::shared_ptr<Scene>& scene, const std::shared_ptr<Context>& context, const std::unordered_map<uint32_t, std::shared_ptr<Node>>& nodes);

        auto GetNodes(ComponentType type) const -> SceneSlice;

        auto Unwrap() const -> std::vector<std::shared_ptr<Node>>;

    private:
        void addNode(const std::shared_ptr<Node>& node);

        std::shared_ptr<Context> m_context;

        std::shared_ptr<Scene> m_scene;

        std::vector<std::shared_ptr<Node>> m_nodes;

        friend class SceneRequester;
    };

    explicit SceneRequester(const std::shared_ptr<Context>& context);
    ~SceneRequester() = default;
    SceneRequester(const SceneRequester&) = delete;
    SceneRequester(SceneRequester&&) = delete;
    SceneRequester& operator=(const SceneRequester&) = delete;
    SceneRequester& operator=(SceneRequester&&) = delete;

    auto GetNodes(const std::shared_ptr<Scene>& scene, ComponentType type) -> SceneSlice;

private:
    std::shared_ptr<Context> m_context;
};

}
