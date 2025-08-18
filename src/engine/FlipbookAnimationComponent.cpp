#include "FlipbookAnimationComponent.h"
#include "MaterialComponent.h"
#include "Context.h"
#include "SceneStore.h"
#include "Scene.h"

#include <algorithm>

namespace engine {

FlipbookAnimationComponent::FlipbookAnimationComponent(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    Component(id, name, owner_node, owner_scene)
{
}

void FlipbookAnimationComponent::init()
{

}

void FlipbookAnimationComponent::update(uint64_t dt)
{
    if (!m_run) {
        return;
    }

    m_from_last_update += dt;

    if (m_from_last_update >= m_update_time) {
        m_from_last_update = 0;

        for (size_t i = 0; i < m_material_ids.size(); ++i) {
            auto ctx = context().lock();
            if (!ctx) {
                return;
            }

            auto scene = ctx->sceneStore->get(ownerScene());
            if (!scene.has_value()) {
                return;
            }

            auto component = scene.value()->getComponent(m_material_ids[i]);
            if (!component.has_value()) {
                return;
            }

            auto material = std::dynamic_pointer_cast<MaterialComponent>(component.value());
            if (!material) {
                return;
            }

            material->setActive(m_current_material == i);
        }

        m_current_material = (m_current_material + 1) % m_material_ids.size();
    }
}

bool FlipbookAnimationComponent::isDirty() const
{
    return true;
}

void FlipbookAnimationComponent::markDirty()
{

}

void FlipbookAnimationComponent::clearDirty()
{

}

std::string FlipbookAnimationComponent::type() const
{
    return "flipbook_animation";
}

void FlipbookAnimationComponent::start()
{
    m_run = true;
}

void FlipbookAnimationComponent::stop()
{
    m_run = false;
}

bool FlipbookAnimationComponent::isRunning() const
{
    return m_run;
}

void FlipbookAnimationComponent::addMaterial(uint32_t material_id)
{
    m_material_ids.push_back(material_id);
}

void FlipbookAnimationComponent::addMaterial(const std::string& material_name)
{
    auto ctx = context().lock();
    if (!ctx) {
        return;
    }

    auto scene = ctx->sceneStore->get(ownerScene());
    if (!scene.has_value()) {
        return;
    }

    auto component = scene.value()->getComponent(material_name);
    if (!component.has_value()) {
        return;
    }

    auto material = std::dynamic_pointer_cast<MaterialComponent>(component.value());
    if (!material) {
        return;
    }

    m_material_ids.push_back(material->id());
}

auto FlipbookAnimationComponent::materialIds() const -> const std::vector<uint32_t>&
{
    return m_material_ids;
}

void FlipbookAnimationComponent::removeMaterial(uint32_t material_id)
{
    auto it = std::find(m_material_ids.begin(), m_material_ids.end(), material_id);
    if (it != m_material_ids.end()) {
        m_material_ids.erase(it);
    }
}

void FlipbookAnimationComponent::setUpdateTime(uint64_t update_time)
{
    m_update_time = update_time;
}

auto FlipbookAnimationComponent::updateTime() const -> uint64_t
{
    return m_update_time;
}

}
