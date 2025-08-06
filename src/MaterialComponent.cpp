#include "MaterialComponent.h"

namespace engine {

MaterialComponent::MaterialComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene),
    m_dirty(true)
{
}

void MaterialComponent::init()
{

}

void MaterialComponent::update(uint64_t dt)
{

}

[[nodiscard]]
bool MaterialComponent::isDirty() const
{
    return m_dirty;
}

void MaterialComponent::markDirty()
{
    m_dirty = true;
}

void MaterialComponent::clearDirty()
{
    m_dirty = false;
}

std::string MaterialComponent::type() const
{
    return "material";
}

void MaterialComponent::setShader(uint32_t shader_id)
{
    m_shader_id = shader_id;
    m_dirty = true;
}

void MaterialComponent::setTexture(uint32_t texture_id)
{
    m_texture_id = texture_id;
    m_dirty = true;
}

auto MaterialComponent::shader() const -> uint32_t
{
    return m_shader_id;
}

auto MaterialComponent::texture() const -> uint32_t
{
    return m_texture_id;
}

}
