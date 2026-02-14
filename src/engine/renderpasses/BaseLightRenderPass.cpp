#include "BaseLightRenderPass.h"

#include "Logger.h"
#include "Context.h"
#include "Scene.h"
#include "Node.h"
#include "LightSourceComponent.h"
#include "CameraComponent.h"
#include "TransformComponent.h"
#include "RenderScopeComponent.h"
#include "Context.h"
#include "RenderPassStore.h"

#include <ranges>
#include <unordered_set>

namespace engine {

void BaseLightRenderPass::render(const std::shared_ptr<Context>& context,
                                 const std::shared_ptr<Scene>& scene,
                                 const std::shared_ptr<Node>& node,
                                 const std::shared_ptr<CameraComponent>& camera) const
{
    Logger::info(__FUNCTION__);

    auto light_source = SceneRequesterHelper::getComponent<LightSourceComponent>(scene, scene->getComponents() | std::views::keys | std::ranges::to<std::unordered_set<uint32_t>>());
    if (!light_source.has_value()) {
        return;
    }

    auto light_source_node = scene->getNode(light_source.value()->ownerNode());
    auto light_source_node_transform = SceneRequesterHelper::getComponent<TransformComponent>(scene, light_source_node.value()->components());
    if (!light_source_node_transform.has_value()) {
        return;
    }

    auto light_source_position = light_source_node_transform.value()->getPosition();
    auto light_source_color = light_source.value()->color();
    auto light_source_intensity = light_source.value()->intensity();

    auto render_scope_component = SceneRequesterHelper::getComponent<RenderScopeComponent>(scene, node->components());
    if (!render_scope_component.has_value()) {
        return;
    }

    auto render_data = render_scope_component.value()->renderData();
    render_data.uniforms["light_color"] = light_source_color;
    render_data.uniforms["light_intensity"] = light_source_intensity;
    render_data.uniforms["light_position"] = light_source_position;
    render_scope_component.value()->setRenderData(render_data);

    auto base_render_pass = context->renderPassStore->get("base_render_pass");
    if (!base_render_pass.has_value()) {
        return;
    }

    base_render_pass.value()->render(context, scene, node, camera);
}

}
