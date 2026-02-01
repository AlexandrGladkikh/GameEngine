#include "RenderPassComponent.h"
#include "Utils.h"

#include <memory>

namespace engine {

RenderPassComponent::RenderPassComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene)
{
}

void RenderPassComponent::init()
{
}

void RenderPassComponent::update(uint64_t dt)
{
}

bool RenderPassComponent::isDirty() const
{
    return false;
}

void RenderPassComponent::markDirty()
{
}

void RenderPassComponent::clearDirty()
{
}

auto RenderPassComponent::type() const -> std::string
{
    return "render_pass";
}

auto RenderPassComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<RenderPassComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();

    clone_component->setRenderPassName(m_render_pass_name);

    return clone_component;
}

void RenderPassComponent::setRenderPassName(const std::string& render_pass_name)
{
    m_render_pass_name = render_pass_name;
}

auto RenderPassComponent::renderPassName() const -> const std::string&
{
    return m_render_pass_name;
}

}