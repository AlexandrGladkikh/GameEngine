#include "Component.h"
#include "Context.h"
#include "SceneStore.h"
#include "Scene.h"

namespace engine {

Component::Component(uint32_t id, const std::string& name, uint32_t owner_node, uint32_t owner_scene) :
    m_id(id),
    m_name(name),
    m_owner_node(owner_node),
    m_owner_scene(owner_scene)
{

}

void Component::setContext(const std::weak_ptr<Context>& context)
{
        m_context = context;
}

auto Component::context() const -> std::weak_ptr<Context>
{
    return m_context;
}

void Component::setName(const std::string &name)
{
    m_name = name;
}

bool Component::isActive() const
{
    return m_is_active;
}

void Component::setActive(bool active)
{
    m_is_active = active;
}

std::optional<std::shared_ptr<Node>> Component::getNode() const
{
    auto context = m_context.lock();
    if (!context) {
        return std::nullopt;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return std::nullopt;
    }

    return scene.value()->getNode(m_owner_node);
}

uint32_t Component::id() const
{
    return m_id;
}

const std::string& Component::name() const
{
    return m_name;
}

uint32_t Component::ownerNode() const
{
    return m_owner_node;
}

uint32_t Component::ownerScene() const
{
    return m_owner_scene;
}

bool Component::isValid() const
{
    return m_is_valid;
}

void Component::update(uint64_t dt)
{
}

void Component::setValid(bool valid)
{
    m_is_valid = valid;
}

}
