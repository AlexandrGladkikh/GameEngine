#include "MeshComponent.h"
#include "Context.h"
#include "MeshStore.h"

namespace engine {

MeshComponent::MeshComponent(const std::string& meshId) :
    m_id(meshId)
{
}

void MeshComponent::bind() const
{
    auto ctx = context().lock();
    if (!ctx) {
        return;
    }

    auto mesh =ctx->meshStore->get(m_id);

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