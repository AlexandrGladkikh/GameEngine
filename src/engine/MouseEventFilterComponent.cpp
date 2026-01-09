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
    registerMouseHandler();

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
    auto ctx = context().lock();
    if (ctx != nullptr) {
        ctx->inputManager->unregisterMouseHandler(m_key);
    }
    m_key = key;
    registerMouseHandler();
}

void MouseEventFilterComponent::setAction(int action)
{
    m_action = action;
}

void MouseEventFilterComponent::setMouseClickCallback(const std::function<void(int, int)>& callback)
{
    m_mouse_click_callback = callback;
}

void MouseEventFilterComponent::clearMouseClickCallback()
{
    m_mouse_click_callback = nullptr;
}

void MouseEventFilterComponent::registerMouseHandler()
{
    auto ctx = context().lock();

    if (ctx != nullptr) {
        ctx->inputManager->registerMouseHandler(m_key, [this](int action, int x, int y) {
                Logger::info("{}: mouse click cb", __FUNCTION__);
                if (action == m_action) {
                    auto node_scale = m_transform->getScale();
                    auto texture_size = m_material->textureSize();
                    texture_size.first *= std::fabs(node_scale.x) / 2.0f;
                    texture_size.second *= std::fabs(node_scale.y) / 2.0f;

                    auto absolute_node_position = m_node_positioning_helper->getAbsoluteNodePosition();
                    if (!absolute_node_position.has_value()) {
                        return;
                    }
                    auto absolute_node_position_value = absolute_node_position.value();

                    float rotation_z_degrees = m_transform->getRotation().z;
                    if (auto node = getNode(); node.has_value()) {
                        auto parent_node = node.value()->getParentNode();
                        while (parent_node.has_value()) {
                            if (auto parent_node_transform = parent_node.value()->getComponent<TransformComponent>(); parent_node_transform.has_value()) {
                                rotation_z_degrees += parent_node_transform.value()->getRotation().z;
                            }
                            parent_node = parent_node.value()->getParentNode();
                        }
                    }

                    const float theta = glm::radians(rotation_z_degrees);
                    const float c = std::cos(theta);
                    const float s = std::sin(theta);

                    const float dx = static_cast<float>(x) - absolute_node_position_value.first;
                    const float dy = static_cast<float>(y) - absolute_node_position_value.second;

                    const float lx = dx * c + dy * s;
                    const float ly = -dx * s + dy * c;

                    if (std::fabs(lx) <= texture_size.first && std::fabs(ly) <= texture_size.second) {
                        Logger::info("{}: mouse click on {}", __FUNCTION__, id());
                        if (m_mouse_click_callback) {
                            m_mouse_click_callback(x, y);
                        }
                    }
                }
            });
    }
}

}
