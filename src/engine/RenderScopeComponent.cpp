#include "RenderScopeComponent.h"
#include "Utils.h"

namespace engine {

RenderScopeComponent::RenderScopeComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene)
{
}

void RenderScopeComponent::init()
{
}

void RenderScopeComponent::update(uint64_t dt)
{
}

[[nodiscard]]
bool RenderScopeComponent::isDirty() const
{
    return false;
}

void RenderScopeComponent::markDirty()
{

}

void RenderScopeComponent::clearDirty()
{
}

auto RenderScopeComponent::type() const -> std::string
{
    return "render_scope";
}

auto RenderScopeComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<RenderScopeComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();

    clone_component->setRenderData(m_render_data);

    return clone_component;
}

auto RenderScopeComponent::isSprite() const -> bool
{
    return m_render_data.is_sprite;
}

void RenderScopeComponent::setIsSprite(bool is_sprite)
{
    m_render_data.is_sprite = is_sprite;
}   

auto RenderScopeComponent::renderData() const -> const RenderData&
{
    return m_render_data;
}

void RenderScopeComponent::setRenderData(const RenderData& render_data)
{
    m_render_data = render_data;
}

auto RenderScopeComponent::renderData(const std::string& name) const -> std::optional<std::any>
{
    auto it = m_render_data.uniforms.find(name);
    if (it == m_render_data.uniforms.end()) {
        return std::nullopt;
    }
    return it->second;
}

void RenderScopeComponent::addRenderData(const std::string& name, const std::any& value)
{
    m_render_data.uniforms[name] = value;
}

}
