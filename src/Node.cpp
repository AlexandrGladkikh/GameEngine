#include "Node.h"


namespace engine {

Node::Node(std::uint32_t id, const std::string& name, uint32_t parent) :
    m_id(id),
    m_name(name),
    m_parent(parent)
{}

std::uint32_t Node::id() const
{
    return m_id;
}

std::string Node::name() const
{
    return m_name;
}

uint32_t Node::parent() const
{
    return m_parent;
}

std::unordered_set<uint32_t> Node::children() const
{
    return m_children_id;
}

std::unordered_set<uint32_t> Node::components() const
{
    return m_components_id;
}

bool Node::addChild(uint32_t id)
{
    return m_children_id.insert(id).second;
}

bool Node::addComponent(uint32_t id)
{
    return m_components_id.insert(id).second;
}

bool Node::removeChild(uint32_t id)
{
    return m_children_id.erase(id) == 1;
}

bool Node::removeComponent(uint32_t id)
{
    return m_components_id.erase(id) == 1;
}

}