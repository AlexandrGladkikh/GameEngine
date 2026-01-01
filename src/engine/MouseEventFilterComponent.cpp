#include "MouseEventFilterComponent.h"
#include "Node.h"
#include "TransformComponent.h"
#include "MaterialComponent.h"
#include "Logger.h"
#include "Utils.h"
#include "InputManager.h"
#include "SceneRequester.h"
#include "NodePositioningHelper.h"

#include "GLFW/glfw3.h"

namespace engine {

MouseEventFilterComponent::MouseEventFilterComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene)
{
}

void MouseEventFilterComponent::init()
{
    auto ctx = context().lock();

    if (ctx != nullptr) {
        ctx->inputManager->registerMouseHandler(m_key, [this](int action, int x, int y) {
            Logger::info("{}: mouse click cb", __FUNCTION__);
            if (action == m_action) {
                mouse_cursor_x_pos = x;
                mouse_cursor_y_pos = y;

                m_update_mouse_position = true;
            }
        });
    }

    auto node = getNode();

    if (!node.has_value()) {
        Logger::error("{}: node no exists", __FUNCTION__);
        return;
    }

    auto node_value = node.value();

    auto transform = node_value->getComponent<TransformComponent>();
    auto material = node_value->getComponent<MaterialComponent>();

    if (!transform.has_value() || !material.has_value()) {
        Logger::error("{}: transform or material no exist", __FUNCTION__);
        return;
    }

    m_transform = transform.value();
    m_material = material.value();
    m_node_positioning_helper = std::make_shared<NodePositioningHelper>(context().lock(), node_value);
}

void MouseEventFilterComponent::update(uint64_t dt)
{
    if (!m_update_mouse_position) {
        return;
    }
    m_update_mouse_position = false;

    auto node_scale = m_transform->getScale();
    auto texture_size = m_material->textureSize();
    texture_size.first *= std::fabs(node_scale.x) / 2.0f;
    texture_size.second *= std::fabs(node_scale.y) / 2.0f;

    auto absolute_node_position = m_node_positioning_helper->getAbsoluteNodePosition();
    if (!absolute_node_position.has_value()) {
        return;
    }
    auto absolute_node_position_value = absolute_node_position.value();

    if ((absolute_node_position_value.first - texture_size.first < mouse_cursor_x_pos && mouse_cursor_x_pos < absolute_node_position_value.first + texture_size.first) &&
       (absolute_node_position_value.second - texture_size.second < mouse_cursor_y_pos && mouse_cursor_y_pos < absolute_node_position_value.second + texture_size.second)) {
        Logger::info("{}: mouse click on {}", __FUNCTION__, id());
    }
}

[[nodiscard]]
bool MouseEventFilterComponent::isDirty() const
{
    return false;
}

void MouseEventFilterComponent::markDirty()
{

}

void MouseEventFilterComponent::clearDirty()
{

}

[[nodiscard]]
std::string MouseEventFilterComponent::type() const
{
    return "mouse_event_filter";
}

auto MouseEventFilterComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<MouseEventFilterComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();
    clone_component->setKey(m_key);
    clone_component->setAction(m_action);
    clone_component->init();

    return clone_component;
}

int MouseEventFilterComponent::key() const
{
    return m_key;
}

int MouseEventFilterComponent::action() const
{
    return m_action;
}

void MouseEventFilterComponent::setKey(int key)
{
    m_key = key;
}

void MouseEventFilterComponent::setAction(int action)
{
    m_action = action;
}

}
