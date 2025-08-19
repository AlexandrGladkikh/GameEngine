#include "MaterialComponent.h"
#include "Context.h"
#include "TextureStore.h"
#include "Texture.h"
#include "ShaderStore.h"
#include "Shader.h"
#include "Utils.h"

namespace engine {

MaterialComponent::MaterialComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene)
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

auto MaterialComponent::type() const -> std::string
{
    return "material";
}

auto MaterialComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<MaterialComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();

    clone_component->setShader(m_shader_id);
    clone_component->setTexture(m_texture_id);

    return clone_component;
}

void MaterialComponent::setShader(uint32_t shader_id)
{
    auto ctx = context().lock();

    if (!ctx || ctx->shaderStore->contains(shader_id)) {
        m_shader_id = shader_id;
        m_dirty = true;
        setValid(true);
    } else {
        setValid(false);
    }
}

void MaterialComponent::setShader(const std::string& shader_name)
{
    const auto shader = context().lock()->shaderStore->getIdByName(shader_name);
    if (shader.has_value()) {
        m_shader_id = shader.value();
        m_dirty = true;
        setValid(true);
    } else {
        setValid(false);
    }
}

void MaterialComponent::setTexture(uint32_t texture_id)
{
    auto ctx = context().lock();

    if (!ctx || ctx->textureStore->contains(texture_id)) {
        m_texture_id = texture_id;
        m_dirty = true;
        setValid(true);
    } else {
        setValid(false);
    }
}

void MaterialComponent::setTexture(const std::string& texture_name)
{
    const auto texture = context().lock()->textureStore->getIdByName(texture_name);
    if (texture.has_value()) {
        m_texture_id = texture.value();
        m_dirty = true;
        setValid(true);
    } else {
        setValid(false);
    }
}

auto MaterialComponent::shaderId() const -> uint32_t
{
    return m_shader_id;
}

auto MaterialComponent::textureId() const -> uint32_t
{
    return m_texture_id;
}

auto MaterialComponent::shaderName() const -> std::string
{
    const auto shader = context().lock()->shaderStore->get(m_shader_id);
    if (!shader.has_value()) {
        return "";
    }

    return shader.value()->name();
}

auto MaterialComponent::textureName() const -> std::string
{
    const auto texture = context().lock()->textureStore->get(m_texture_id);
    if (!texture.has_value()) {
        return "";
    }

    return texture.value()->name();
}

auto MaterialComponent::textureSize() const -> std::pair<uint32_t, uint32_t>
{
    const auto texture = context().lock()->textureStore->get(m_texture_id);
    if (!texture.has_value()) {
        return {0, 0};
    }

    return {texture.value()->width(), texture.value()->height()};
}


}
