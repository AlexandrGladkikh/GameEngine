#include "MeshComponent.h"
#include "Context.h"
#include "MeshStore.h"

namespace engine {

MeshComponent::MeshComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene),
    m_dirty(true)
{
}

void MeshComponent::init()
{

}

void MeshComponent::update(uint64_t dt)
{

}

[[nodiscard]]
bool MeshComponent::isDirty() const
{
    return m_dirty;
}

void MeshComponent::markDirty()
{
    m_dirty = true;
}

void MeshComponent::clearDirty()
{
    m_dirty = false;
}

std::string MeshComponent::type() const
{
    return "mesh";
}

void MeshComponent::bind() const
{
    auto ctx = context().lock();
    if (!ctx) {
        return;
    }

    auto mesh = ctx->meshStore->get(m_id);

    if (mesh.has_value()) {
        mesh.value()->bind();
    }
}

void MeshComponent::unbind() const
{
    auto ctx = context().lock();
    if (!ctx) {
        return;
    }

    auto mesh =ctx->meshStore->get(m_id);

    if (mesh.has_value()) {
        mesh.value()->unbind();
    }
}

void MeshComponent::setMesh(uint32_t meshId)
{
    m_id = meshId;
    markDirty();
}

void MeshComponent::setMesh(const std::string& mesh_name)
{
    auto ctx = context().lock();
    if (!ctx) {
        return;
    }

    auto mesh = ctx->meshStore->getIdByName(mesh_name);

    if (mesh.has_value()) {
        m_id = mesh.value();
        markDirty();
    }
}

uint32_t MeshComponent::meshId() const
{
    return m_id;
}

auto MeshComponent::meshName() const -> std::string
{
    auto ctx = context().lock();
    if (!ctx) {
        return "";
    }

    auto mesh = ctx->meshStore->get(m_id);
    if (!mesh.has_value()) {
        return "";
    }

    return mesh.value()->name;
}

}