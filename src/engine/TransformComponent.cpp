#include "TransformComponent.h"
#include "Utils.h"

#include <glm/ext/matrix_transform.hpp>

namespace engine {

TransformComponent::TransformComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene),
    m_dirty(true),
    m_model(glm::mat4(1.0f)),
    m_position(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_rotation(glm::vec3(0.0f, 0.0f, 0.0f)),
    m_scale(glm::vec3(1.0f, 1.0f, 1.0f))
{

}

void TransformComponent::init()
{

}

void TransformComponent::update(uint64_t dt)
{

}

bool TransformComponent::isDirty() const
{
    return m_dirty;
}

void TransformComponent::markDirty()
{
    m_dirty = true;
}

void TransformComponent::clearDirty()
{
    m_dirty = false;
}

auto TransformComponent::type() const -> std::string
{
    return "transform";
}

auto TransformComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<TransformComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();

    clone_component->setPosition(m_position);
    clone_component->setRotation(m_rotation);
    clone_component->setScale(m_scale);

    return clone_component;
}

glm::mat4 TransformComponent::getModel()
{
    if (m_dirty) {
        m_dirty = false;
        m_model = glm::mat4(1.0f);
        m_model = glm::translate(m_model, m_position);
        m_model = glm::rotate(m_model, glm::radians(m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        m_model = glm::rotate(m_model, glm::radians(m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        m_model = glm::rotate(m_model, glm::radians(m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        m_model = glm::scale(m_model, m_scale);

        clearDirty();
    }

    return m_model;
}

void TransformComponent::setPosition(const glm::vec3& position)
{
    m_position = position;
    markDirty();
}

void TransformComponent::setRotation(const glm::vec3& rotation)
{
    m_rotation = rotation;
    markDirty();
}

void TransformComponent::setScale(const glm::vec3& scale)
{
    m_scale = scale;
    markDirty();
}

glm::vec3 TransformComponent::getPosition() const
{
    return m_position;
}

glm::vec3 TransformComponent::getRotation() const
{
    return m_rotation;
}

glm::vec3 TransformComponent::getScale() const
{
    return m_scale;
}

}
