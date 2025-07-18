#include "Component.h"

namespace engine {

Component::Component(uint32_t id, const std::string& name, uint32_t owner) :
    m_id(id),
    m_name(name),
    m_owner(owner)
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

bool Component::isActive() const
{
    return m_is_active;
}

void Component::setActive(bool active)
{
    m_is_active = active;
}

uint32_t Component::id() const
{
    return m_id;
}

const std::string& Component::name() const
{
    return m_name;
}

uint32_t Component::owner() const
{
    return m_owner;
}

void Component::update(uint64_t dt)
{
}

}