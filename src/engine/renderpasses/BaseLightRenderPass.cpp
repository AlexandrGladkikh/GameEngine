#include "BaseLightRenderPass.h"

#include "Logger.h"
#include "Context.h"
#include "Scene.h"
#include "Node.h"
#include "CameraComponent.h"

namespace engine {

void BaseLightRenderPass::render(const std::shared_ptr<Context>& context,
                                 const std::shared_ptr<Scene>& scene,
                                 const std::shared_ptr<Node>& node,
                                 const std::shared_ptr<CameraComponent>& camera) const
{
    Logger::debug(__FUNCTION__);
}

}