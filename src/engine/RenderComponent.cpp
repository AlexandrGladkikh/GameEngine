#include "RenderComponent.h"
#include "Utils.h"

namespace engine {

RenderComponent::RenderComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene),
    m_sprite(false)
{
}

void RenderComponent::init()
{
}

void RenderComponent::update(uint64_t dt)
{
}

[[nodiscard]]
bool RenderComponent::isDirty() const
{
    return false;
}

void RenderComponent::markDirty()
{

}

void RenderComponent::clearDirty()
{
}

auto RenderComponent::type() const -> std::string
{
    return "render";
}

auto RenderComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<RenderComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();

    clone_component->setSprite(m_sprite);

    return clone_component;
}

auto RenderComponent::isSprite() const -> bool
{
    return m_sprite;
}

void RenderComponent::setSprite(bool sprite)
{
    m_sprite = sprite;
}

}
