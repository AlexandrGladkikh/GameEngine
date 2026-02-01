#include "Renderer.h"
#include "Context.h"
#include "Scene.h"
#include "SceneRequester.h"
#include "SceneRequesterHelper.h"
#include "TransformComponent.h"
#include "CameraComponent.h"
#include "Logger.h"
#include "Node.h"
#include "renderpasses/BaseRenderPass.h"
#include "RenderPassStore.h"
#include "RenderPassComponent.h"

namespace engine {

void Renderer::render(const std::shared_ptr<Context>& context, const std::shared_ptr<Scene>& scene)
{
    Logger::info(__FUNCTION__);

     glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
     glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    static auto requester = SceneRequester(context);
    auto nodes = requester.GetNodes(scene, ComponentType::Mesh).
                                                 GetNodes(ComponentType::Material).
                                                 GetNodes(ComponentType::Transform).Unwrap();

    auto camera_nodes = requester.GetNodes(scene, ComponentType::Camera).GetNodes(ComponentType::Transform).Unwrap();
    if (camera_nodes.empty()) {
        return;
    }

    auto camera_node = camera_nodes.front();

    if (!camera_node || !camera_node->isActive()) {
        return;
    }

    auto camera_component = SceneRequesterHelper::getComponent<CameraComponent>(scene, camera_node->components());
    if (!camera_component.has_value()) {
        return;
    }

    auto camera_node_transform = SceneRequesterHelper::getComponent<TransformComponent>(scene, camera_node->components());
    if (!camera_node_transform.has_value()) {
        return;
    }

    auto camera_position = camera_node_transform.value()->getPosition();
    auto camera = camera_component.value();
    camera->setPosition(camera_position);

    for (const auto& node : nodes) {
        if (!node->isActive()) {
            continue;
        }

        auto render_pass_component = SceneRequesterHelper::getComponent<RenderPassComponent>(scene, node->components());
        if (!render_pass_component.has_value()) {
            continue;
        }

        auto render_pass_name = render_pass_component.value()->renderPassName();
        auto render_pass = context->renderPassStore->get(render_pass_name);
        if (!render_pass.has_value()) {
            continue;
        }

        render_pass.value()->render(context, scene, node, camera);
    }
}

}
