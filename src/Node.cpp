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

auto buildNode(rapidjson::Value& node_json) -> std::optional<std::unique_ptr<Node>>
{
    auto id = node_json["id"].GetUint();
    auto name = node_json["name"].GetString();
    auto parent = node_json["parent"].GetUint();

    auto node = std::make_shared<Node>(id, name, parent);

    auto children = node_json["children"].GetArray();
    for (auto& child : children) {
        node->addChild(child.GetUint());
    }

    auto components = node_json["components"].GetArray();
    for (auto& component : components) {
        node->addComponent(component.GetUint());
    }
    return std::make_unique<Node>(id, name, parent);
}

void saveNode(const std::shared_ptr<Node>& node, rapidjson::Value& node_json, rapidjson::Document::AllocatorType& allocator)
{
    node_json.AddMember("id", node->id(), allocator);
    node_json.AddMember("name", node->name(), allocator);
    node_json.AddMember("parent", node->parent(), allocator);

    rapidjson::Value children(rapidjson::kArrayType);
    for (const auto child : node->children()) {
        children.PushBack(child, allocator);
    }
    node_json.AddMember("children", children, allocator);

    rapidjson::Value components(rapidjson::kArrayType);
    for (const auto component : node->components()) {
        components.PushBack(component, allocator);
    }
    node_json.AddMember("components", components, allocator);
}

}