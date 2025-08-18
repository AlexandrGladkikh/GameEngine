#include "Node.h"
#include "Logger.h"
#include "Utils.h"
#include "Component.h"

namespace engine {

Node::Node(std::uint32_t id, const std::string& name, uint32_t parent, uint32_t owner_scene) :
    m_id(id),
    m_name(name),
    m_parent(parent),
    m_owner_scene(owner_scene)
{}

void Node::setContext(const std::weak_ptr<Context>& context)
{
    m_context = context;
}

std::uint32_t Node::id() const
{
    return m_id;
}

std::string Node::name() const
{
    return m_name;
}

uint32_t Node::getParentId() const
{
    return m_parent;
}

uint32_t Node::ownerScene() const
{
    return m_owner_scene;
}

auto Node::getScene() const -> std::optional<std::shared_ptr<Scene>>
{
    auto context = m_context.lock();
    if (!context) {
        return nullptr;
    }

    return context->sceneStore->get(m_owner_scene);
}

auto Node::getParentNode() const -> std::optional<std::shared_ptr<Node>>
{
    auto context = m_context.lock();
    if (!context) {
        return std::nullopt;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return std::nullopt;
    }

    return scene.value()->getNode(m_parent);
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

auto Node::addChild(const std::string& name) -> std::shared_ptr<Node>
{
    auto context = m_context.lock();
    if (!context) {
        return nullptr;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return nullptr;
    }

    auto newNode = std::make_shared<Node>(generateUniqueId(), name, m_id, m_owner_scene);
    newNode->setContext(m_context);
    addChild(newNode->id());
    scene.value()->addNode(newNode->id(), newNode);

    return newNode;
}

bool Node::addComponent(uint32_t id)
{
    return m_components_id.insert(id).second;
}

auto Node::addComponent(const std::string& type, const std::string& name) -> std::optional<std::shared_ptr<Component>>
{
    if (hasComponent(type)) {
        return std::nullopt;
    }

    auto component = ComponentBuilder::buildEmptyComponent(type, name, id(), m_owner_scene);

    if (!component.has_value()) {
        return std::nullopt;
    }

    component.value()->setContext(m_context);

    auto context = m_context.lock();
    if (!context) {
        return std::nullopt;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return std::nullopt;
    }

    auto component_id = component.value()->id();
    scene.value()->addComponent(component_id, std::move(component.value()));

    addComponent(component_id);

    return scene.value()->getComponent(component_id).value();
}

bool Node::removeChild(uint32_t id)
{
    return m_children_id.erase(id) == 1;
}

bool Node::removeComponent(uint32_t id)
{
    auto context = m_context.lock();
    if (!context) {
        return false;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return false;
    }

    scene.value()->removeComponent(id);

    return m_components_id.erase(id) == 1;
}

auto Node::getChild(uint32_t id) const -> std::optional<std::shared_ptr<Node>>
{
    auto context = m_context.lock();
    if (!context) {
        return std::nullopt;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return std::nullopt;
    }

    return scene.value()->getNode(id);
}

auto Node::getChild(const std::string& name) const -> std::optional<std::shared_ptr<Node>>
{
    auto context = m_context.lock();
    if (!context) {
        return std::nullopt;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return std::nullopt;
    }

    return scene.value()->getNode(name);
}

auto Node::getComponent(uint32_t id) const -> std::optional<std::shared_ptr<Component>>
{
    auto context = m_context.lock();
    if (!context) {
        return std::nullopt;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return std::nullopt;
    }

    return scene.value()->getComponent(id);
}

bool Node::hasComponent(const std::string& type) const
{
    auto context = m_context.lock();
    if (!context) {
        return false;
    }

    auto scene = context->sceneStore->get(m_owner_scene);
    if (!scene.has_value()) {
        return false;
    }

    for (auto component_id : components()) {
        auto component = scene.value()->getComponent(component_id);
        if (component.has_value() && component.value()->type() == type) {
            return true;
        }
    }

    return false;
}

auto buildNode(rapidjson::Value& node_json) -> std::optional<std::unique_ptr<Node>>
{
    Logger::info(__FUNCTION__);

    auto id = node_json["id"].GetUint();
    auto name = node_json["name"].GetString();
    auto parent = node_json["parent"].GetUint();
    auto owner_scene = node_json["owner_scene"].GetUint();

    Logger::info("id: {}, name: {}, parent: {}, owner_scene: {}", id, name, parent, owner_scene);

    auto node = std::make_unique<Node>(id, name, parent, owner_scene);

    auto children = node_json["children"].GetArray();
    for (auto& child : children) {
        node->addChild(child.GetUint());
    }

    auto components = node_json["components"].GetArray();
    for (auto& component : components) {
        node->addComponent(component.GetUint());
    }
    return std::move(node);
}

void saveNode(const std::shared_ptr<Node>& node, rapidjson::Value& node_json, rapidjson::Document::AllocatorType& allocator)
{
    rapidjson::Value value;
    value.SetString(node->name().c_str(), allocator);
    node_json.AddMember("id", node->id(), allocator);
    node_json.AddMember("name", value, allocator);
    node_json.AddMember("parent", node->getParentId(), allocator);
    node_json.AddMember("owner_scene", node->ownerScene(), allocator);

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