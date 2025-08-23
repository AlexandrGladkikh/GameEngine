#pragma once

#include "Context.h"
#include "SceneStore.h"
#include "Scene.h"
#include "SceneRequesterHelper.h"
#include "ComponentBuilder.h"

#include <rapidjson/document.h>

#include <cstdint>
#include <string>
#include <unordered_set>
#include <memory>
#include <optional>

namespace engine {

class Component;
struct Context;

class Node final {
public:
    explicit Node(std::uint32_t id, const std::string& name, uint32_t parent, uint32_t owner_scene);
    ~Node() = default;
    Node(const Node&) = delete;
    Node(Node&&) = delete;
    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&) = delete;

    [[nodiscard]]
    bool isActive() const;
    void setActive(bool active);

    void setContext(const std::weak_ptr<Context>& context);

    std::uint32_t id() const;
    std::string name() const;
    uint32_t getParentId() const;

    uint32_t ownerScene() const;

    auto getScene() const -> std::optional<std::shared_ptr<Scene>>;

    auto getParentNode() const -> std::optional<std::shared_ptr<Node>>;

    auto clone(uint32_t owner_node_id) const -> std::optional<std::shared_ptr<Node>>;

    std::unordered_set<uint32_t> children() const;
    std::unordered_set<uint32_t> components() const;

    void setName(const std::string& name);

    bool addChild(uint32_t id);
    auto addChild(const std::string& name) -> std::shared_ptr<Node>;
    bool addComponent(uint32_t id);
    auto addComponent(const std::string& type, const std::string& name) -> std::optional<std::shared_ptr<Component>>;

    bool removeChild(uint32_t id);
    bool removeComponent(uint32_t id);

    auto getChild(uint32_t id) const -> std::optional<std::shared_ptr<Node>>;
    auto getChild(const std::string& name) const -> std::optional<std::shared_ptr<Node>>;

    auto getComponent(uint32_t id) const -> std::optional<std::shared_ptr<Component>>;

    bool hasComponent(const std::string& type) const;

    template<typename T>
    std::optional<std::shared_ptr<T>> getComponent() const
    {
        auto context = m_context.lock();
        if (!context) {
            return std::nullopt;
        }

        auto scene = context->sceneStore->get(m_owner_scene);
        if (!scene.has_value()) {
            return std::nullopt;
        }

        return SceneRequesterHelper::getComponent<T>(scene.value(), components());
    }

    template<typename T>
    bool hasComponent() const
    {
        auto context = m_context.lock();
        if (!context) {
            return false;
        }

        auto scene = context->sceneStore->get(m_owner_scene);
        if (!scene.has_value()) {
            return false;
        }

        return SceneRequesterHelper::hasComponent<T>(scene.value(), components());
    }

    template<typename T>
    std::optional<std::shared_ptr<T>> addComponent(const std::string& name)
    {
        if (hasComponent<T>()) {
            return std::nullopt;
        }

        auto component = ComponentBuilder::buildEmptyComponent<T>(name, id(), m_owner_scene);

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

        return std::static_pointer_cast<T>(scene.value()->getComponent(component_id).value());
    }

private:
    bool m_is_active = true;

    std::weak_ptr<Context> m_context;

    std::uint32_t m_id;
    std::string m_name;
    uint32_t m_parent;
    uint32_t m_owner_scene;

    std::unordered_set<uint32_t> m_children_id;
    std::unordered_set<uint32_t> m_components_id;
};

auto buildNode(rapidjson::Value& node_json) -> std::optional<std::unique_ptr<Node>>;
void saveNode(const std::shared_ptr<Node>& node, rapidjson::Value& node_json, rapidjson::Document::AllocatorType& allocator);

}
