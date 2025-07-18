#include "MaterialComponent.h"

namespace engine {

MaterialComponent::MaterialComponent(uint32_t id, const std::string& name, uint32_t owner,
    uint32_t shader_id, uint32_t texture_id) :
    Component(id, name, owner),
    m_shader_id(shader_id),
    m_texture_id(texture_id)
{
}

auto MaterialComponent::shader() const
{
    return m_shader_id;
}

auto MaterialComponent::texture() const
{
    return m_texture_id;
}

}
