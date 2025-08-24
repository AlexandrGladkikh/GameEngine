#include "Node.h"
#include "Logger.h"
#include "Utils.h"
#include "Component.h"
#include "UserComponentsBuilder.h"

namespace engine {

Node::Node(std::uint32_t id, const std::string& name, uint32_t parent, uint32_t owner_scene) :
    m_id(id),
    m_name(name),
    m_parent(parent),
    m_owner_scene(owner_scene)
{}

bool Node::isActive() const
{
    return m_is_active;
}

void Node::setActive(bool active)
{
    m_is_active = active;

    for (auto component_id : m_components_id) {
        auto component = getComponent(component_id);
        if (component.has_value()) {
            component.value()->setActive(m_is_active);
        }
    }

    for (auto child_id : m_children_id) {
        auto child = getChild(child_id);
        if (child.has_value()) {
            child.value()->setActive(active);
        }
    }
}

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

auto Node::clone(uint32_t owner_node_id) const -> std::optional<std::shared_ptr<Node>>
{
    auto scene = getScene();
    if (!scene.has_value()) {
        return std::nullopt;
    }

    const auto& scene_value = scene.value();

    auto clone_node = std::make_shared<Node>(generateUniqueId(), m_name, owner_node_id, m_owner_scene);
    clone_node->setContext(m_context);

    for (const auto& child_id : m_children_id) {
        auto child = getChild(child_id);
        if (!child.has_value()) {
            return std::nullopt;
        }

        auto child_clone = child.value()->clone(clone_node->id());
        if (!child_clone.has_value()) {
            return std::nullopt;
        }

        clone_node->addChild(child_clone.value()->id());
        scene_value->addNode(child_clone.value()->id(), std::move(child_clone.value()));
    }

    for (const auto& component_id : m_components_id) {
        auto component = getComponent(component_id);
        if (!component.has_value()) {
            return std::nullopt;
        }

        auto component_clone = component.value()->clone(clone_node->id());
        clone_node->addComponent(component_clone->id());
        auto id = component_clone->id();
        scene_value->addComponent(id, std::move(component_clone));
    }

    scene_value->addNode(clone_node->id(), clone_node);
    auto owner_node = scene_value->getNode(owner_node_id);
    if (!owner_node.has_value()) {
        return std::nullopt;
    }
    owner_node.value()->addChild(clone_node->id());

    return clone_node;
}

std::unordered_set<uint32_t> Node::children() const
{
    return m_children_id;
}

std::unordered_set<uint32_t> Node::components() const
{
    return m_components_id;
}

void Node::setName(const std::string &name)
{
    m_name = name;
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

    auto context = m_context.lock();
    if (!context) {
        return std::nullopt;
    }
    if (!component.has_value()) {
        component = context->userComponentsBuilder->buildEmptyComponent(type, name, id(), m_owner_scene);
    }

    if (!component.has_value()) {
        return std::nullopt;
    }

    component.value()->setContext(m_context);

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

    scene.value()->getComponent(component_id).value()->init();

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
