#pragma once

#include <memory>

namespace engine {

class Node;
struct Context;
class Scene;
class CameraComponent;

class RenderPass {
public:
    explicit RenderPass() = default;
    virtual ~RenderPass() = default;

    virtual void render(const std::shared_ptr<Context>& context,
                        const std::shared_ptr<Scene>& scene,
                        const std::shared_ptr<Node>& node,
                        const std::shared_ptr<CameraComponent>& camera) const = 0;
};

}