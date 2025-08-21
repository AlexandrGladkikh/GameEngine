#include "FlipbookAnimationComponent.h"
#include "MaterialComponent.h"
#include "Context.h"
#include "SceneStore.h"
#include "Scene.h"
#include "Utils.h"
#include "Helpers.h"

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
    if (!m_run || m_material_ids.empty()) {
        return;
    }

    m_from_last_update += dt;

    if (m_from_last_update >= m_update_time) {
        m_from_last_update = 0;

        for (size_t i = 0; i < m_material_ids.size(); ++i) {
            auto ctx = context().lock();
            if (!ctx) {
                break;
            }

            auto scene = ctx->sceneStore->get(ownerScene());
            if (!scene.has_value()) {
                break;
            }

            auto component = scene.value()->getComponent(m_material_ids[i]);
            if (!component.has_value()) {
                break;
            }

            auto material = std::dynamic_pointer_cast<MaterialComponent>(component.value());
            if (!material) {
                break;
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

auto FlipbookAnimationComponent::type() const -> std::string
{
    return "flipbook_animation";
}

auto FlipbookAnimationComponent::clone(uint32_t owner_node_id) const -> std::unique_ptr<Component>
{
    auto clone_component = std::make_unique<FlipbookAnimationComponent>(generateUniqueId(), name(), owner_node_id, ownerScene());
    clone_component->setContext(context());
    clone_component->setValid(isValid());
    clone_component->setActive(isActive());
    clone_component->markDirty();

    clone_component->setUpdateTime(m_update_time);

    for (size_t i = 0; i < m_material_ids.size(); ++i) {
        clone_component->addMaterial(m_material_ids[i]);
    }

    if (m_run) {
        clone_component->start();
    }

    return clone_component;
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
    if (hasMaterial(material_id)) {
        return;
    }
    m_material_ids.push_back(material_id);
}

void FlipbookAnimationComponent::addMaterial(const std::string& material_name)
{
    auto material = engine::getComponentByName<MaterialComponent>(this, material_name);
    if (!material) {
        return;
    }

    if (hasMaterial(material->id())) {
        return;
    }
    m_material_ids.push_back(material->id());
}

bool FlipbookAnimationComponent::hasMaterial(uint32_t material_id) const
{
    return std::find(m_material_ids.begin(), m_material_ids.end(), material_id) != m_material_ids.end();
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

void FlipbookAnimationComponent::replaceMaterial(uint32_t material_id, uint32_t new_material_id)
{
    if (hasMaterial(new_material_id)) {
        return;
    }
    auto it = std::find(m_material_ids.begin(), m_material_ids.end(), material_id);
    if (it != m_material_ids.end()) {
        *it = new_material_id;
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
