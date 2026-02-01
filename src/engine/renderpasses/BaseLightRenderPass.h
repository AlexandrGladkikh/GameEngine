#pragma once

#include "RenderPass.h"

namespace engine {

class BaseLightRenderPass final : public RenderPass {
public:
    explicit BaseLightRenderPass() = default;
    ~BaseLightRenderPass() override = default;

    void render(const std::shared_ptr<Context>& context,
                const std::shared_ptr<Scene>& scene,
                const std::shared_ptr<Node>& node,
                const std::shared_ptr<CameraComponent>& camera) const override;
};

}
