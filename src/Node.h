#pragma once

#include "Context.h"
#include "SceneStore.h"
#include "Scene.h"
#include "SceneRequesterHelper.h"

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

    void setContext(const std::weak_ptr<Context>& context);

    std::uint32_t id() const;
    std::string name() const;
    uint32_t parent() const;

    std::unordered_set<uint32_t> children() const;
    std::unordered_set<uint32_t> components() const;

    bool addChild(uint32_t id);
    bool addComponent(uint32_t id);

    bool removeChild(uint32_t id);
    bool removeComponent(uint32_t id);

    template<typename T>
    std::optional<std::shared_ptr<T>> getComponent() const
    {
        return SceneRequesterHelper::getComponent<T>(m_context.lock()->sceneStore->get(m_owner_scene).value(), components());
    }

private:
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
