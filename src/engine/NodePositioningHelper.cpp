#include "NodePositioningHelper.h"

#include "Node.h"
#include "TransformComponent.h"
#include "MaterialComponent.h"
#include "Logger.h"
#include "SceneRequester.h"
#include "SceneRequesterHelper.h"
#include "CameraComponent.h"

namespace engine {

NodePositioningHelper::NodePositioningHelper(const std::weak_ptr<Context>& context, const std::shared_ptr<Node>& node) :
    m_context(context),
    m_node(node)
{
    auto transform = m_node->getComponent<TransformComponent>();
    auto material = m_node->getComponent<MaterialComponent>();

    if (!transform.has_value() || !material.has_value()) {
        Logger::error("{}: transform or material no exist", __FUNCTION__);
        return;
    }

    m_transform = transform.value();
    m_material = material.value();
}

std::optional<std::pair<float, float>> NodePositioningHelper::getAbsoluteNodePosition() const
{
    if (!m_transform || !m_material) {
        Logger::error("{}: transform or material no exist", __FUNCTION__);
        return std::nullopt;
    }

    auto scene = m_node->getScene();

    if (!scene.has_value()) {
        Logger::error("{}: scene no exists", __FUNCTION__);
        return std::nullopt;
    }

    auto scene_value = scene.value();

    auto requester = SceneRequester(m_context.lock());
    auto nodes = requester.GetNodes(scene_value, ComponentType::Mesh).
                 GetNodes(ComponentType::Material).
                 GetNodes(ComponentType::Transform).Unwrap();

    auto camera_nodes = requester.GetNodes(scene_value, ComponentType::Camera).GetNodes(ComponentType::Transform).Unwrap();
    if (camera_nodes.empty()) {
        return std::nullopt;
    }

    auto camera_node = camera_nodes.front();

    if (!camera_node || !camera_node->isActive()) {
        return std::nullopt;
    }

    auto camera_component = SceneRequesterHelper::getComponent<CameraComponent>(scene_value, camera_node->components());
    if (!camera_component.has_value()) {
        return std::nullopt;
    }

    auto camera_node_transform = SceneRequesterHelper::getComponent<TransformComponent>(scene_value, camera_node->components());
    if (!camera_node_transform.has_value()) {
        return std::nullopt;
    }

    auto camera_position = camera_node_transform.value()->getPosition();

    auto model_mtx = m_transform->getModel();

    glm::vec3 absolute_node_position = m_transform->getPosition();

    auto parent = m_node->getParentNode();
    while (parent.has_value()) {
        if (!parent.value()->hasComponent<TransformComponent>()) {
            parent = parent.value()->getParentNode();
            continue;
        }

        auto transform_parent = parent.value()->getComponent<TransformComponent>();
        if (!transform_parent.has_value() || !transform_parent.value()->isActive()) {
            break;
        }

        model_mtx = transform_parent.value()->getModel() * model_mtx;
        absolute_node_position += transform_parent.value()->getPosition();
        parent = parent.value()->getParentNode();
    }

    float absoluteNodePositionX = absolute_node_position.x - camera_position.x;
    float absoluteNodePositionY = absolute_node_position.y - camera_position.y;

    return std::make_pair(absoluteNodePositionX, absoluteNodePositionY);
}

}
