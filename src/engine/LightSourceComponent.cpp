#include "LightSourceComponent.h"
#include "Utils.h"

namespace engine {

LightSourceComponent::LightSourceComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene),
    m_color(glm::vec3(1.0f, 1.0f, 1.0f)),
    m_intensity(1.0f)
{
}

void LightSourceComponent::init()
{
}

void LightSourceComponent::update(uint64_t dt)
{
}

bool LightSourceComponent::isDirty() const
{
    return false;
}

void LightSourceComponent::markDirty()
{
}

void LightSourceComponent::clearDirty()
{
}

auto LightSourceComponent::type() const -> std::string
{
    return "light_source";
}

auto LightSourceComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<LightSourceComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();

    clone_component->setColor(m_color);
    clone_component->setIntensity(m_intensity);

    return clone_component;
}

void LightSourceComponent::setColor(const glm::vec3& color)
{
    m_color = color;
}

void LightSourceComponent::setIntensity(float intensity)
{
    m_intensity = intensity;
}

auto LightSourceComponent::color() const -> glm::vec3
{
    return m_color;
}

auto LightSourceComponent::intensity() const -> float
{
    return m_intensity;
}

}