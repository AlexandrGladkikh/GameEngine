#include "MeshComponent.h"
#include "Context.h"
#include "MeshStore.h"

namespace engine {

MeshComponent::MeshComponent(uint32_t id, const std::string& name, uint32_t owner,
    uint32_t meshId) :
    Component(id, name, owner),
    m_id(meshId),
    m_dirty(true)
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

}